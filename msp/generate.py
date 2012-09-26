
import subprocess

def generate(dev):


def commit(dev):
    make_env = os.environ.copy
    make_env["DEVICE"] = dev["location"]
    subprocess.call("make program")

