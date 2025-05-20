import sys

def load(path):
    with open(path) as f:
        return [float(x) for x in f.read().split()]

def main():
    ec = load('/tmp/ec_core_out.txt')
    eg = load('/tmp/eigen_core_out.txt')
    ec = ec[:8]
    eg = eg[:8]
    if len(ec) != len(eg):
        print('length mismatch')
        sys.exit(1)
    for a, b in zip(ec, eg):
        if abs(a - b) > 1e-12:
            print('mismatch', a, b)
            sys.exit(1)
    print('parity OK')

if __name__ == '__main__':
    main()
