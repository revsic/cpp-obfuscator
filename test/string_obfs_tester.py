import sys

def main(argc, argv):
    if argc < 3:
        print('USAGE: python string_obfs_tester.py [STRING_OBFS.EXE] [TARGET_STRING]')
        return 1
    
    path = argv[1]
    with open(path, 'rb') as f:
        data = f.read()
    
    if data.find(str.encode(argv[2])) == -1:
        print('Cannot find string "{}" from "{}"'.format(argv[2], argv[1]))
        return 0
    else:
        print('Find string "{}" from "{}"'.format(argv[2], argv[1]))
        return 1

if __name__ == '__main__':
    retn = main(len(sys.argv), sys.argv)
    sys.exit(retn)
