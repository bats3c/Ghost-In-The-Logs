# Ghost In The Logs

This tool allows you to evade sysmon and windows event logging, my blog post about it can be found [here](https://blog.dylan.codes/evading-sysmon-and-windows-event-logging/)

## Usage

You can grab the lastest release [here](https://github.com/bats3c/Ghost-In-The-Logs/releases/download/1.0/gitl.exe)

#### Starting off

Once you've got the latest version execute it with no arguments to see the avalible commands

    $ gitl.exe

![alt text](https://raw.githubusercontent.com/bats3c/Ghost-In-The-Logs/master/res/gitl_exec_no_args.jpg "executed with no args")

#### Loading the hook

    $ gitl.exe load

![alt text](https://raw.githubusercontent.com/bats3c/Ghost-In-The-Logs/master/res/gitl_load_hook.jpg "loaded the hook")

#### Enabling the hook (disabling all logging)

    $ gitl.exe enable

![alt text](https://raw.githubusercontent.com/bats3c/Ghost-In-The-Logs/master/res/gitl_enable_hook.jpg "enabled the hook")

#### Disabling the hook (enabling all logging)

    $ gitl.exe disable

![alt text](https://raw.githubusercontent.com/bats3c/Ghost-In-The-Logs/master/res/gitl_disable_hook.jpg "disabled the hook")

#### Get status of the hook

    $ gitl.exe status

![alt text](https://raw.githubusercontent.com/bats3c/Ghost-In-The-Logs/master/res/gitl_hook_status.jpg "hook status")

## Prerequisites

*   High integrity administrator privilages

## Credits

Huge thanks to:

*   hfiref0x for the amazing [KDU](https://github.com/hfiref0x/KDU)
*   everdox for the super cool [InfinityHook](https://github.com/everdox/InfinityHook)
