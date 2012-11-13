#!/bin/zsh
#
# vi-mode colour changing
#   http://www.zsh.org/mla/users/2006/msg01196.html

setopt prompt_subst
autoload colors
colors

rst="%{%b%s%u$reset_color%}"
bgc="%{%(?.$rst.%S)%}"

function lprompt {
    local col1 col2 ch1 ch2
    col1="%{%b$fg[$2]%}"
    col2="%{$4$fg[$3]%}"
    ch1=$col1${1[1]}
    ch2=$col1${1[2]}

    local git_b git_s col_b col_s
    git_b='$(get_git_prompt_info '%b')'
    git_s='$(get_git_prompt_info '%s')'
    if [ -n "$TALIA_SHELL" ]; then talia_s=" [talia] " ; fi
    if [ -n "$SSH_TTY" ]; then ssh_s="*" ; fi
    col_s="%{$fg[red]%}"

    PROMPT="\
$col2$ssh_s \
$bgc$ch1\
$bgc$col2%B%1~%b\
$col1 %n\
$col_s$talia_s\
$ch2$rst\
$col2\$$rst "
}

function _rprompt_dir {
    local col_b col_s
    git_p=${$(get_git_prompt_info '%p')%% }
    col_p="%{$fg[yellow]%}"

    local short
    short="${PWD/$HOME/~}"

    if test -z "$git_p" ; then
            echo -n "$short"
            return
    fi

    local lead rest
    lead=$git_p
    rest=${${short#$lead}#/}

    echo -n "$lead$col_p/$rest"
}

function rprompt {
    local col1 col2 ch1 ch2
    col1="%{$fg[$2]%}"
    col2="%{$4$fg[$3]%}"
    ch1=$col1${1[1]}
    ch2=$col1${1[2]}
    col_git="%{$fg[green]%}"
    col_rvm="%{$fg[blue]%}"
    git_b='$(get_git_prompt_info '%b' )'
    rvm_b='$(get_rvm_prompt_info '%b' )'

    local _dir='$(_rprompt_dir)'
    
    if [[ ( "$TERM" = "screen" ) && ( -n "$STY" ) ]]; then screen_s="screen:$STY[(ws:.:)-1] " ; fi

    RPROMPT="\
$rst$ch1\
$col1$screen_s\
$col_git$git_b\
$col_rvm$rvm_b\
$col2%n@%m\
$col1:\
$col2%B$_dir%b\
$ch2$rst"
}

if [ $UID -eq 0 ]; then
    lprompt '[]' red red
    rprompt '[]' black black
elif [ "$TERM" = "screen" ]; then
    lprompt '' green blue
    rprompt '[]' blue black
elif [ -n "$SSH_TTY" ]; then
    lprompt '' cyan blue
    rprompt '[]' black black
else
    lprompt '' blue black
    rprompt '[]' black black
fi


unset rst bgc

# ------------------------------
# http://dotfiles.org/~frogb/.zshrc

case $TERM in
    xterm* | rxvt* | urxvt*)
        precmd() { 
                print -Pn "\e]0;%n@%m: %~\a"
        }
        preexec() {
                #print -Pn "\e]0;$1\a"
                print -Pn "\e]0;%n@%m: %~  $1\a"
        }
        ;;
    screen*)
        precmd() { 
                print -nR $'\033k'"zsh"$'\033'\\\

                print -nR $'\033]0;'"zsh"$'\a'
        }
        preexec() {
                print -nR $'\033k'"$1"$'\033'\\\

                print -nR $'\033]0;'"$1"$'\a'
        }
        ;;
esac

