#LS
case "$(uname)" in
    Linux)
        alias ls='ls --color=auto' ;;
    FreeBSD)
        alias ls='ls -G' ;;
esac

alias l='ls -lFh'     #size,show type,human readable
alias la='ls -lAFh'   #long list,show almost all,show type,human readable

#EDITORS
alias vi='vim'

#COLORFULNESS
alias grep="grep --colour"

#no vim to .vim corretion and expandig of aliases after sudo (tailing space)
#alias sudo='nocorrect sudo '

#DIRS
alias -g ...='../../'           #cd ...
alias -g ....='../../../'       #cd ....
alias -g .....='../../../../'   #cd .....

#sprunge paste service
alias sprunge="curl -F 'sprunge=<-' http://sprunge.us"

#Just some bullshit
alias shutdown="sudo shutdown now -HP"
alias r8='echo "gr8 m8 i r8 $((RANDOM % 8))/8"'
alias serverstart=' sudo service ssh restart && sudo service dnsmasq restart && sudo service tftpd-hpa restart'
alias serverstop='sudo service dnsmasq stop && sudo service tftpd-hpa stop'
#added to make erasing easier
scriptdir=/home/hdd-test-server/HDD/scripts
#use on client
alias erase="sudo ./erase_start.zsh"
alias eraseauto="sudo ./erase_start.zsh -a "
alias smartcheck="sudo ./hdd_test_smart.zsh default sata"
#use on server
alias pullLogs="sudo $scriptdir/pull_logs.zsh"
alias Hupdate="sudo $scriptdir/oldscripts/homeupdate.zsh"

