import subprocess
import time

process = subprocess.Popen(['tail', '/tmp/datos.dat'], stdout=subprocess.PIPE)
stdout = process.communicate()[0]

while True:
    while True:
        try:
            output = process.stdout.readline()
        except:
            output = ''
        if output == '' and process.poll() is not None:
            time.sleep(1)
            break
        if output:
            print "."
            print output.strip()
        print "+"
        rc = process.poll()




