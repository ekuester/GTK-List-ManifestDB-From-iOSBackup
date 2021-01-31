#!/bin/bash
linguas="po/LINGUAS"
program="ManifestDBView"
IFS=$'\n'
for lingua in `cat $linguas`; do
  [[ "$lingua" =~ ^#.*$ ]] && continue
  echo "$lingua.po read from $linguas"
  rm -frv $lingua
  mkdir -p "$lingua/LC_MESSAGES"
  msgfmt po/$lingua.po -o "$lingua/LC_MESSAGES/$program.mo"
  echo "po/$lingua/LC_MESSAGES/$program.mo generated"
done

