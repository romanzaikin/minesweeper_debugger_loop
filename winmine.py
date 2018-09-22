from pydbg import *
from pydbg.defines import *


def win(dbg):
    dbg.set_register("ESI",1)
    return DBG_CONTINUE


dbg = pydbg()

for pid,name in dbg.enumerate_processes():
    if name == "winmine.exe":
        dbg.attach(pid)

dbg.bp_set(0x01003488, handler=win)

dbg.run()