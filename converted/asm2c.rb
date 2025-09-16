#!/usr/bin/ruby

require 'debug'

TYPES = Hash[*%w[
    cond     int
    ax       int
    al       int
    ah       int
    bx       int
    bl       int
    bh       int
    cx       int
    cl       int
    ch       int
    dx       int
    dl       int
    dh       int
    si       int
    di       int
    es       int
    fac0     real
    fac1     real
    fac2     real
    fac3     real
    fac4     real
    fac5     real
    fac6     real
    fac7     real
    fac8     real
    fac9     real
    fac10    real
    fac11    real
    fac12    real
    f0       real
    f1       real
    f2       real
    bp_2     int
    bp_4     int
    bp_6     int
    bp_8     int
    bp_10    int
    bp_12    int
]]

class AbstractStatement
  def translate(context);  end
  def conditionals?; false; end
  def flag_affecting?; false; end
  def as_c; end
  def print; end
end

class ContextStatement < AbstractStatement
  attr_accessor :context
  def initialize(context)
    @context = context
  end
  def translate(_context)
    @context.translate
  end
  def as_c
    @context.as_c
  end
  def print; end
end

class Statement < AbstractStatement
  attr_accessor :line, :lineno, :label, :op, :oprands, :tailcomment, :headcomment, :error
  attr_accessor :translated, :translated_oprands
  def print
    if @error
      "error: %s" % @line
    elsif @headcomment
      "// #{@headcomment}"
    else
      "%-12s%-10s%-30s// %s" % [@label, @op, @oprands&.join(','), @tailcomment]
    end
  end
  def as_c
    label = @label ? "  #{@label.downcase}:\n" : ""
    if %w[proc endp segment ends struc db dw equ].include?(@op&.downcase)
      label = ""
    end
    if @error
      "// parse_error: #{@line}"
    elsif @headcomment
      "// #{@headcomment}"
    else
      if @translated && @translated.size > 40
        "%s    %s\n    %-40.40s// %s" % [label, @translated, "", @line]
      else
        "%s    %-40.40s// %s" % [label, @translated || "", @line]
      end
    end
  end
  def resolve_operand(opr)
    opr = opr.downcase
    opr = opr.sub(/<?type (vector|environment|real)>?/) {
      case $1
      when "vector"; "12"
      when "real"; "4"
      when "environment"; "(sizeof environment)"
      end
    }
    prefix, suffix = opr.split(/\./, 2)
    resolved = case prefix
               when /(word|byte) ptr -(\d+)\[(\w+)\]/
                 "#{$3}_#{$2}" # -2[bp] -> bp_2
               when /^-env\[bp\]$/
                 "bp_env#{$1}"
               when /^(?:(?:word|byte) ptr )?([a-z]+)\[(bx|si|di|bp)\]$/
                 "#{$1}[#{$2}]"
               when /^(?:(?:word|byte) ptr )?\[(bx|si|di|bp)\]$/
                 "(*#{$1})"
               when /^([abcd][lhx]|si|di|bp)$/
                 prefix
               when /^\d+$/
                 prefix
               when /^([0-9][0-9a-fA-F]*)h$/
                 "0x#{$1}"
               when /^offset \w+:(\w+)$/
                 "&(#{$1})"
               else
                 prefix.sub(/^(?:(?:word|byte) ptr )/, '')
               end
    if suffix
      if /^\(\*(\w+)\)$/ =~ resolved
        resolved = "#{$1}->#{suffix}"
      else
        resolved += "." + suffix
      end
    end
    resolved
  end
  def conditionals?
    if /^n?j(?<n>n?)(?<op>[abclgzs]?[e]?)$/ =~ @op&.downcase
      cond = case op
               when "b", "c", "l", "s"; "xxx < yyy"
               when "be", "le";         "xxx <= yyy"
               when "a", "g";           "xxx > yyy"
               when "ae", "ge";         "xxx >= yyy"
               when "e", "z";           "xxx == yyy"
               else "unknown"
             end
      if n == "n"
        cond = "!(#{cond})"
      end
      cond
    end
  end
  def flag_affecting?
    case @op&.downcase
    when "add", "sub", "neg", "cmp"; true
    when "fp0?", "fpsgn?"; true
    when "fpcmp"; true
    when "or", "and", "xor"; true
    when "inc", "dec"; true
    else false
    end
  end
  def set_flags(cond, context)
    ro0, ro1 = @translated_oprands
    ro1 ||= "0"
    context.seen("cond")
    set_cond = case @op&.downcase
               when "fp0?"
                 "cond = (" + cond.sub('yyy', "0").sub('xxx', "((#{ro0}).man & 0x8000)") + ")"
               when "fpsgn?"
                 "cond = (" + cond.sub('xxx', "(#{ro0}).sign").sub('yyy', "0") + ")"
               when "inc", "dec"
                 "/cond = (" + cond.sub('xxx', "(#{ro0})").sub('yyy', "0") + ")"
               when "or", "and", "neg"
                 # assume jz ar jnz
                 "cond = (" + cond.sub('xxx', "(#{ro0})").sub('yyy', "0") + ")"
               when "fpcmp"
                 "/cond = (" + cond.sub('xxx', "ax").sub('yyy', "0") + ")"
               else
                 "cond = (" + cond.sub('xxx', "(#{ro0})").sub('yyy', "(#{ro1})") + ")"
               end
    if set_cond[0] == '/'
      @translated =  "#{@translated} #{set_cond[1..-1]};"
    else
      @translated =  "#{set_cond}; #{@translated}"
    end
  end
  def create_context?
    case @op&.downcase
    when "proc"; Procedure.new(@label)
    else false
    end
  end
  def end_context?
    case @op&.downcase
    when "endp"; @label
    when "end"; "toplevel"
    end
  end

  def translate(context)
    ro = @translated_oprands = @oprands&.map{|o| resolve_operand(o)}
    ro0, ro1, ro2 = ro
    if conditionals?
      @translated = "if (cond) {goto #{ro0};}"
    else
      case @op&.downcase
      when "segment", "ends", "db", "dw", "real", "equ"
        @label = nil
      when "mov"
        context.seen(ro0)
        @translated = "#{ro0} = #{ro1};"
      when "lea"
        context.seen(ro0)
        @translated = "#{ro0} = &(#{ro1});"
      when "add"
        context.seen(ro0)
        @translated = "#{ro0} += #{ro1};"
      when "sub"
        context.seen(ro0)
        @translated = "#{ro0} -= #{ro1};"
      when "shl"
        context.seen(ro0)
        @translated = "#{ro0} <<= #{ro1};"
      when "cmp"
        context.seen(ro0)
        @translated = ""
      when "neg"
        context.seen(ro0)
        @translated = "#{ro0} = - #{ro0};"
      when "inc"
        context.seen(ro0)
        @translated = "#{ro0} += 1;"
      when "dec"
        context.seen(ro0)
        @translated = "#{ro0} -= 1;"
      when "or"
        context.seen(ro0)
        # usually check whether ro0 is 0 or not
      when "xor"
        context.seen(ro0)
        if ro0 == ro1
          @translated = "#{ro0} = 0;"
        else
          @translated = "#{ro0} ^= #{ro1};"
        end
      when "fpadd", "fpsub", "fpmlt", "fpdiv"
        context.seen(ro0)
        f = op.downcase[-3..-1]
        @translated = "f#{f}(&#{ro0}, &#{ro1});"
      when "fpsqrt", "fpsqar", "fpinv", "fpexp", "fplog"
        context.seen(ro0)
        f = op.downcase[2..-1]
        @translated = "f#{f}(&#{ro0});"
      when "fpcmp"
        context.seen(ro0);
        @translated = "ax = fcomp(&#{ro0}, &#{ro1});"
      when "fitor"
        context.seen(ro0)
        @translated = "itor(&#{ro0}, ax);"
      when "frtoi", "frtoif"
        context.seen(ro0);
        f = op.downcase[1..-1]
        @translated = "ax = #{f}(&#{ro0});"
      when "fpscale"
        context.seen(ro0)
        @translated = "fscale(&#{ro0}, #{ro1});"
      when "fpabs"
        context.seen(ro0)
        @translated = "#{ro0}.sign = 0;"
      when "fpsgn?", "fp0?"
        context.seen(ro0);
      when "fpchs"
        context.seen(ro0)
        @translated = "#{ro0}.sign ^= 1;"
      when "fplod"
        context.seen(ro0)
        @translated = "#{ro0}.man = #{ro1}; #{ro0}.exp = #{ro2}; #{ro0}.sign = #{ro[3]};"
      when "movmem"
        if (ro2.nil? || ro2 == "4")
          context.seen(ro0)
          @translated = "#{ro0} = #{ro1};"
        elsif (ro2 == "12")
          ro0.sub!(/\.x$/, '')
          ro1.sub!(/\.x$/, '')
          @translated = "#{ro0}.x = #{ro1}.x; #{ro0}.y = #{ro1}.y; #{ro0}.z = #{ro1}.z;"
        else
          @translated = "movmem(#{ro0}, #{ro1}, #{ro2});"
        end
      when "call"
        @translated = "#{ro0}();"
      when "link", "unlk"
      when "ret"
        @translated = "goto endp;"
      when "clc", "stc"
        @translated = "/* carry flag #{op} */"
      when "jmp"
        @translated = "goto #{ro0};"
      when "jcxz"
        @translated = "if (cx == 0) {goto #{ro0};}"
      when "loop"
        @translated = "if (--cx != 0) {goto #{ro0};}"
      end
    end
  end
end

class Context
  attr_accessor :name, :statements, :seen
  def initialize(name)
    @name = name
    @statements = []
    @seen = {}
  end
  def add_statement(stmt)
    @statements << stmt
  end
  def seen(var)
    @seen[var] = true
  end
  def inner_context(stmt)
  end
  def translate
    @statements.each.with_index do |stmt, i|
      stmt.translate(self)
      if stmt.is_a?(ContextStatement)
        inner_context(stmt)
      end
      cond = stmt.conditionals?
      if cond
        found = false
        # search for flag-setting insn upto 3 insns prior
        (1..3).each do |j|
          if i-j >= 0
            ast = @statements[i-j]
            if ast.flag_affecting?
              ast.set_flags(cond, self)
              found = true
              break
            end
          end
        end
        unless found
          stmt.translated += " /* condition not resolved */"
        end
      end
    end
  end
  def as_c
    lines = []
    lines += prologue
    lines += @statements.map(&:as_c)
    lines += epilogue
    lines << ""
    lines.join("\n")
  end
  def prologue; []; end
  def epilogue; []; end
end

class TopLevel < Context
  def prologue
    [<<EOL]
#include "trace.h"

EOL
  end
  def epilogue; []; end
end
class Procedure < Context
  attr_accessor :inner_procedures
  def initialize(name)
    super
    @inner_procedures = []
  end
  def inner_context(stmt)
    @inner_procedures << stmt
  end
  def as_c
    lines = []
    lines += prologue
    lines += @statements.find_all{|stmt| ! @inner_procedures.include?(stmt)}.map(&:as_c)
    lines += epilogue
    lines << ""
    lines += @inner_procedures.map(&:as_c)
    lines << ""
    lines.join("\n")
  end
  def prologue
    vars = {}
    @seen.keys.compact.sort.each do |v|
      if t = TYPES[v]
        vars[t] ||= []
        vars[t] << v
      end
    end
    [
      "/*\n * #{@name}\n */\n",
      "void #{@name.downcase}() {",
      *vars.keys.sort.map{|t| "#{t} #{vars[t].join(', ')};"},
      ""
    ]
  end
  def epilogue
    [
      "  endp:;",
      "}"
    ]
  end
end

def parse(line, lineno)
  stmt = Statement.new
  stmt.line = line
  stmt.lineno = lineno
  if /^\s*;+(?<head>.*)$|^\s*$/ =~ line
    stmt.headcomment = head
    return stmt
  end

  if /\s+(?<tail>;.*)/ =~ line
    stmt.tailcomment = tail
    line.gsub!(tail, '')
  end

  if /^(?<label>[@\w]+:?)?\s*(?<op>[\w.?]+)\s*(?<args>.*?)?\s*$/ =~ line
    stmt.label = label&.sub(/:$/, '')
    stmt.op = op
    stmt.oprands = args&.scan(/(<[^>]+>|[^,]+)/).flatten
    return stmt
  end
  stmt.error = 'unknown syntax'
  stmt
end

def process_file(src, dst)
  contexts = [TopLevel.new("toplevel")]
  File.open(src, "r") do |s|
    lineno = 0
    s.each do |line|
      line.sub!(/\r?\n$/, '')
      stmt = parse(line, lineno += 1)

      new_ctx = stmt.create_context?
      if new_ctx
        contexts.push(new_ctx)
        # fall through: add context start to new context
      end

      contexts.last.add_statement(stmt)

      # check end context after adding endp stmt
      end_ctx = stmt.end_context?
      if end_ctx
        if end_ctx != contexts.last.name
          STDERR.puts "endp context mismatch: expected #{contexts.last.name} actual #{end_ctx}"
        end
        if contexts.size >= 2
          ended = contexts.pop
          contexts.last.add_statement(ContextStatement.new(ended))
        end
      end
    end
  end
  toplevel = ContextStatement.new(contexts.first)
  File.open(dst, "w") do |d|
    toplevel.translate(nil) # dummy param
    d.puts toplevel.as_c.gsub(/[ \t]+\n/, "\n")
  end
end

def main
  if ARGV.size != 2
    STDERR.puts "Usage asm2c.rb src dst"
    exit 1
  end

  src, dst = ARGV
  process_file(src, dst)
end

main if $0 == __FILE__
