# bash completion for mbpoll

_mbpoll()
{
  local cur prev opts
  COMPREPLY=()
  cur="${COMP_WORDS[COMP_CWORD]}"
  prev="${COMP_WORDS[COMP_CWORD-1]}"
  opts="-m -a -r -c -t -l -o -p -b -d -s -P -0 -W -B -1 -q -u -R -F -h -V -v"

  # Completions for -m
  if [[ "$prev" == "-m" ]]; then
    COMPREPLY=( $(compgen -W "rtu tcp" -- "$cur") )
    return 0
  fi

  # Completions for -d
  if [[ "$prev" == "-d" ]]; then
    COMPREPLY=( $(compgen -W "7 8" -- "$cur") )
    return 0
  fi

  # Completions for -s
  if [[ "$prev" == "-s" ]]; then
    COMPREPLY=( $(compgen -W "1 2" -- "$cur") )
    return 0
  fi

  # Completions for -P
  if [[ "$prev" == "-P" ]]; then
    COMPREPLY=( $(compgen -W "none even odd" -- "$cur") )
    return 0
  fi

  # Completions for -t
  if [[ "$prev" == "-t" ]]; then
    COMPREPLY=( $(compgen -W "0 1 3 3:int16 3:hex 3:string 3:int 3:float 4 4:int16 4:hex 4:string 4:int 4:float" -- "$cur") )
    return 0
  fi

  # Complete options
  if [[ "$cur" == -* ]]; then
    COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
    return 0
  fi
}

complete -F _mbpoll mbpoll
