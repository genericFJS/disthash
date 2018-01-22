import pexpect
import getpass
import os
import sys


def ssh_check(host, user, password, timeout=30):
    host_out = host.replace('\r', '').replace('\n', '')
    # cmd = "hostname"
    options = ('-q -oStrictHostKeyChecking=no '
               '-oUserKnownHostsFile=/dev/null -oPubkeyAuthentication=no')
    # ssh_cmd = 'ssh %s@%s %s "%s"' % (user, host, options, cmd)
    ssh_cmd = 'ssh %s@%s %s' % (user, host, options)
    try:
        child = pexpect.spawn(ssh_cmd, timeout=timeout)
        child.expect(['Password: '])

        child.sendline(password)

        # ssh_ret = child.read().decode("utf-8")
        # ssh_ret = ssh_ret.replace('\r', '').replace('\n', '')

        print('{:10s} online'.format(host_out))
        child.close()
    except:
        print('{:10s} offline !!!'.format(host_out))
        return 1

    return 0


if __name__ == "__main__":
    os.chdir(os.path.dirname(os.path.abspath(sys.argv[0])))
    user_inp = input("    user: ")
    pw_inp = getpass.getpass('password: ')

    working_machines = open('../machines/machinefile', 'w')

    with open("../machines/machinefileAll") as f:
        for machine in f:
            if ssh_check(machine, user_inp, pw_inp, 1) == 0:
                working_machines.write(machine)

    working_machines.close()
