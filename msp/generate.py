
import subprocess
import os.path
import struct
from mako.template import Template

autoglue="""

#include "autocode.h"
#include "comm.h"

void comm_set_reg(uint16_t address, uint8_t data){
    if(address < 0x100){
        // reserved
        return;
    }
    % for (var, fmt, address, length, mname) in module_variables:
    //${var}: format ${fmt} @ address ${"{:#x}".format(address)} (${length} bytes)
    if(address < ${address+length}){
        *(((uint8_t*)&${var}) + (address - ${address})) = data;
        return;
    }
    % endfor
}

uint8_t comm_get_reg(uint16_t address){
    if(address < 0x100){
        // reserved
        return 0;
    }
    % for (var, fmt, address, length, mname) in module_variables:
    //${var}: format ${fmt} @ address ${"{:#x}".format(address)} (${length} bytes)
    if(address < ${address+length}){
        return *(((uint8_t*)&${var}) + (address - ${address}));
    }
    % endfor
    return 0;
}

void init_auto(){
    % for initfunc in init_functions:
        ${initfunc}();
    %endfor
}

void loop_auto(){
    % for loopfunc in loop_functions:
        ${loopfunc}();
    %endfor
}
"""

def generate(dev):
    members = dev["members"]
    seqnum = 23
    current_address = 0x0100
    autocode = ""
    init_functions = []
    loop_functions = []
    module_variables = []
    for mname, member in members.items():
        mfile = member["type"]
        mtype = mfile.replace('-', '_')
        typepath = os.path.join(os.path.dirname(__file__), mfile + ".c.tp")
#        if not os.path.exists(typepath):
#            print("Cannot find a handler for member type " + dev["type"])
#            exit(1)
        init_function = "init_{}_{}".format(mtype, seqnum)
        init_functions.append(init_function)
        loop_function = "loop_{}_{}".format(mtype, seqnum)
        loop_functions.append(loop_function)
        member["variables"] = {}
        def modulevar(name, fmt=None):
            varname = "modvar_{}_{}_{}".format(mtype, seqnum, name)
            if fmt is not None:
                slen = struct.calcsize(fmt)
                module_variables.append( (varname, fmt, current_address, slen, mname) )
                current_address += slen 
                member["variables"][name] = {"length": length, "address": address}
            return varname
        seqnum += 1
        tp = Template(filename=typepath)
        # "modulevar": modulevar,
        autocode += tp.render_unicode(init_function=init_function, loop_function=loop_function, modulevar=modulevar, dev=dev)
    autocode += Template(autoglue).render_unicode(init_functions=init_functions, loop_functions=loop_functions, module_variables=module_variables)
    with open(os.path.join(os.path.dirname(__file__), "autocode.c"), "w") as f:
        f.write(autocode)
    subprocess.call(["/usr/bin/env", "make", "-C", os.path.dirname(__file__), "clean", "all"])

def commit(dev):
    make_env = os.environ.copy()
    make_env["SERIAL"] = dev["location"] # "location" contains the launchpad's USB serial number
    subprocess.call(["/usr/bin/env", "make", "-C", os.path.dirname(__file__), "program"], env=make_env)

