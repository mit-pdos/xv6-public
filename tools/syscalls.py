from optparse import OptionParser
import sys, re

def main():
    parser = OptionParser(usage="usage: %prog [options] source...")
    parser.add_option("--kvectors", action="store_true",
                      help="output kernel syscall vectors")
    parser.add_option("--ustubs", action="store_true",
                      help="output user syscall stubs")
    parser.add_option("--udecls", action="store_true",
                      help="output user syscall declarations")
    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        parser.exit()

    # Parse source files
    syscalls = []
    for fname in args:
        syscalls.extend(parse(file(fname, "r")))

    # Generate syscall numbers
    for n, syscall in enumerate(syscalls):
        syscall.num = n + 1

    # Output
    if options.kvectors:
        print "#include \"types.h\""
        print "#include \"userptr.hh\""
        print
        for syscall in syscalls:
            print "extern %s %s(%s);" % (syscall.rettype, syscall.kname,
                                         ", ".join(syscall.kargs))
        print

        print "u64 (*syscalls[])(u64, u64, u64, u64, u64) = {"
        bynum = dict((s.num, s) for s in syscalls)
        for num in range(max(bynum.keys()) + 1):
            if num not in bynum:
                print "  nullptr,"
            else:
                print "  (u64(*)(u64,u64,u64,u64,u64))%s," % bynum[num].kname
        print "};"
        print
        print "extern const int nsyscalls = %d;" % (max(bynum.keys()) + 1)

    if options.ustubs:
        print "#include \"traps.h\""
        print
        for syscall in syscalls:
            print """\
.globl SYS_%(uname)s
SYS_%(uname)s = %(num)d

.globl %(uname)s
%(uname)s:
  movq $%(num)d, %%rax""" % syscall.__dict__
            if "INT" in syscall.flags:
                print "  int $T_SYSCALL"
            else:
                print "  movq %rcx, %r10\n  syscall"
            print "  ret"
            print

    if options.udecls:
        for syscall in syscalls:
            extra = ""
            if "NORET" in syscall.flags:
                extra = " __attribute__((noreturn))"
            print "%s %s(%s)%s;" % (syscall.rettype, syscall.uname,
                                    ", ".join(syscall.uargs), extra)

class Syscall(object):
    def __init__(self, kname, rettype, kargs, flags, num=None):
        self.kname, self.rettype, self.kargs, self.flags, self.num = \
            kname, rettype, kargs, flags, num

        self.basename = kname[4:]

        # Construct user space prototype
        self.uname = self.basename
        uargs = []
        for karg in kargs:
            m = re.match("(.*?) *[a-z_]+$", karg)
            atype = m.group(1)
            while True:
                atype2 = re.sub("userptr<(.*)>", r"\1*", atype)
                if atype2 == atype:
                    break
                atype = atype2
            uargs.append(atype)
        self.uargs = uargs

    def __repr__(self):
        return "Syscall(%r,%r,%r,%r,%r)" % (
            self.kname, self.rettype, self.kargs, self.flags, self.num)

class ParseError(RuntimeError):
    def __init__(self, fname, msg):
        RuntimeError.__init__(self, "%s: %s" % (fname, msg))

def parse(fp):
    res = []

    for flags, proto in re.findall(r"//SYSCALL(.*)([^{]*)", fp.read()):
        # Parse the prototype
        proto = " ".join(proto.split())
        m = re.match(r"(.+) ([a-z_]+) *\(([^)]+)\)", proto)
        if not m:
            raise ParseError(fp.name, "could not parse prototype %r" % proto)
        rettype, name, kargs = m.groups()
        kargs = re.split(" *, *", kargs)

        res.append(Syscall(name, rettype, kargs, flags.split()))

    return res

if __name__ == "__main__":
    main()
