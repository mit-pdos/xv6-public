from optparse import OptionParser
import sys, re

def main():
    parser = OptionParser(usage="usage: %prog [options] source...")
    parser.add_option("--kvectors", action="store_true",
                      help="output kernel syscall vectors")
    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        parser.exit()

    # Parse source files
    syscalls = []
    for fname in args:
        syscalls.extend(parse(file(fname, "r")))

    # Parse syscall numbers
    nums = dict(re.findall("#define SYS_([^ ]*) +([0-9]+)", file("include/syscall.h").read()))
    for syscall in syscalls:
        syscall.num = int(nums[syscall.basename])

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

class Syscall(object):
    def __init__(self, kname, rettype, kargs, num=None):
        self.kname, self.rettype, self.kargs, self.num = \
            kname, rettype, kargs, num

        self.basename = kname[4:]

    def __repr__(self):
        return "Syscall(%r,%r,%r,%r)" % (
            self.kname, self.rettype, self.kargs, self.num)

class ParseError(RuntimeError):
    def __init__(self, fname, msg):
        RuntimeError.__init__(self, "%s: %s" % (fname, msg))

def parse(fp):
    res = []

    for proto in re.findall(r"//SYSCALL\n([^{]*)", fp.read()):
        # Parse the prototype
        proto = " ".join(proto.split())
        m = re.match(r"(.+) ([a-z_]+) *\(([^)]+)\)", proto)
        if not m:
            raise ParseError(fp.name, "could not parse prototype %r" % proto)
        rettype, name, kargs = m.groups()
        kargs = re.split(" *, *", kargs)

        res.append(Syscall(name, rettype, kargs))

    return res

if __name__ == "__main__":
    main()
