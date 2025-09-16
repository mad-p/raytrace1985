#!/bin/bash
subdiv=${1:-1}
arg_subdiv=-a$subdiv
if [[ "$subdiv" = "1" ]]; then
  file_subdiv=
  dir_subdiv=
else
  file_subdiv="_$subdiv"
  dir_subdiv="aa$subdiv/"
  mkdir -p $dir_subdiv
fi


for i in *.sld; do
  case "$i" in
    kaleido*.sld)
      shadow=-S
      ;;
    *)
      shadow=
      ;;
  esac
  echo ../converted/raytrace $arg_subdiv -q $shadow "-o$dir_subdiv${i/\.sld/${file_subdiv}.bmp}" "$i"
  ../converted/raytrace $arg_subdiv -q $shadow "-o$dir_subdiv${i/\.sld/${file_subdiv}.bmp}" "$i"
done
