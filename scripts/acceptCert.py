import pexpect
import getpass
import os
import sys
import socket

machines_online = 0


def ssh_check(host, user, password, timeout=30):
    global machines_online
    host_out = host.replace('\r', '').replace('\n', '')
    # cmd = "hostname"
    options = ('-q -oStrictHostKeyChecking=no '
               '-oUserKnownHostsFile=/dev/null '
               '-oPubkeyAuthentication=no'
               )
    # ssh_cmd = 'ssh %s@%s %s "%s"' % (user, host, options, cmd)
    ssh_cmd = 'ssh %s@%s %s' % (user, host, options)
    # First pass:
    try:
        child = pexpect.spawn(ssh_cmd, timeout=timeout)
    except pexpect.exceptions.TIMEOUT:
        pass
    try:
        child.expect(['Password: '])
        child.sendline(password)
    except pexpect.exceptions.TIMEOUT:
        pass
    child.close()
    # Second pass:
    try:
        child = pexpect.spawn(ssh_cmd, timeout=timeout)
    except pexpect.exceptions.TIMEOUT:
        print('{:10s} OFFLINE (timeout)'.format(host_out))
        return 1
    try:
        child.expect(['Password: '])

        child.sendline(password)
        print('{:10s} online (password entered)'.format(host_out))
        machines_online += 1
        child.close()
    except pexpect.exceptions.TIMEOUT:
        # print(child.before.decode("utf-8"))
        if child.before.decode("utf-8") == "":
            print('{:10s} OFFLINE'.format(host_out))
            return 1
        else:
            print('{:10s} online (key accepted)'.format(host_out))
            machines_online += 1
    child.close()
    return 0


if __name__ == "__main__":
    os.chdir(os.path.dirname(os.path.abspath(sys.argv[0])))
    user_inp = input("    user: ")
    pw_inp = getpass.getpass('password: ')

    working_machines = open('../machines/machinefile', 'w')

    with open("../machines/machinefileAll") as f:
        try:
            for machine in f:
                if ssh_check(machine, user_inp, pw_inp, 0.5) == 0:
                    machine_ip = socket.gethostbyname(machine.strip())
                    # print('\tAdding ' + format(machine_ip) + ' as machine.')
                    working_machines.write(machine_ip + "\n")
            print('Number of machines online:', machines_online)
            with open("../machines/machineCount", 'w') as f:
                f.write(str(machines_online))
        except (KeyboardInterrupt, SystemExit):
            print("Exiting...")

    working_machines.close()
