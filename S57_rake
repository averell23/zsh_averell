# Stolen from original rake task and 
# http://weblog.rubyonrails.org/2006/3/9/fast-rake-task-completion-for-zsh
# and improved

_rake_does_task_list_need_generating () {
  if [ ! -f .rake_tasks ]; then return 0;
  else
    accurate=$(stat -f%m .rake_tasks)
    changed=$(stat -f%m Rakefile)
    return $(expr $accurate '>=' $changed)
  fi
}

_rake () {
  if [ -f Rakefile ]; then
    if _rake_does_task_list_need_generating; then
      echo "\nGenerating .rake_tasks..." > /dev/stderr
      echo "RAKE_COMPLETION=(" > .rake_tasks
      rake --silent --tasks  | sed 's/:/\\\\:/g' | sed 's/rake \([^ ]*\)[ ]*# \(.*\)/\1:"\2"/' >> .rake_tasks
      echo ")" >> .rake_tasks
    fi
    . ./.rake_tasks
    _arguments -C -s -S \
      '(--classic-namespace -C)'{--classic-namespace,-C}'[put Task and FileTask in the top level namespace]' \
      '(--dry-run -n)'{--dry-run,-n}'[do a dry run without executing actions]' \
      '(- *)'{--help,-H}'[display help information]' \
      \*{--libdir,-I}'[include specified directory in the search path for required modules]:library directory:_files -/' \
      '(--nosearch -N)'{--nosearch,-N}'[do not search parent directories for the Rakefile]' \
      '(- *)'{--prereqs,-P}'[display the tasks and prerequisites, then exit]' \
      '(--quiet -q --silent -s --verbose -v)'{--quiet,-q}'[do not log messages to standard output]' \
      '(--rakefile -f)'{--rakefile,-f}'[use specified file as the rakefile]:rake file:_files' \
      \*{--require,-r}'[require specified module before executing rakefile]:library:->library' \
      '(--quiet -q --silent -s --verbose -v)'{--silent,-s}"[like --quiet, but also suppresses the 'in directory' announcement]" \
      '(- *)'{--tasks,-T}'[display the tasks and dependencies, then exit]' \
      '(--trace -t)'{--trace,-t}'[turn on invoke/execute tracing, enable full backtrace]' \
      '(- *)'{--usage,-h}'[display usage information]' \
      '(--quiet -q --silent -s --verbose -v)'{--verbose,-v}'[log message to standard output (default)]' \
      '(- *)'{--version,-V}'[display version information]' \
      '(--describe -D)'{--describe,-D}'[describe the tasks matching the pattern]' \
      '1::task:(($RAKE_COMPLETION))' \
      '*::argument:= _normal' && ret=0
    # compadd `cat .rake_tasks`
  fi
}

compdef _rake rake