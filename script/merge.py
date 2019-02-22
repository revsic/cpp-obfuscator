import os
import re
import sys


class Format(object):
    hpp = '''\
#ifndef {0}
#define {0}

{1}
{2}
{3}
#endif'''

    @classmethod
    def hpp_beutifier(cls, source):
        out = ''
        n_blank = 0
        for line in source.split('\n'):
            if line == '' or line.isspace():
                n_blank += 1
            else:
                if n_blank > 2:
                    n_blank = 2

                out += '\n' * n_blank
                n_blank = 0

                out += line + '\n'
        return out


class ReSupport(object):
    r_guard = re.compile(r'(#ifndef|#endif)')
    r_include_dep = re.compile(r'#include "(.+?)"')
    r_include = re.compile(r'#include <(.+?)>')
    r_define = re.compile(r'#define (.+)')

    @classmethod
    def guard(cls, inp):
        return cls.r_guard.findall(inp)

    @classmethod
    def include_dep(cls, inp):
        return cls.r_include_dep.findall(inp)

    @classmethod
    def include(cls, inp):
        return cls.r_include.findall(inp)

    @classmethod
    def define(cls, inp):
        return cls.r_define.findall(inp)


class SourceInfo(object):
    def __init__(self):
        self.out = ''
        self.deps = []
        self.includes = []
        self.defines = []

    @classmethod
    def set_with(cls, out, deps, includes, defines):
        obj = cls()
        obj.out = out
        obj.deps = deps
        obj.includes = includes
        obj.defines = defines
        return obj

    @classmethod
    def read_file(cls, path):
        obj = cls()
        with open(path) as f:
            for line in f.readlines():
                if len(ReSupport.guard(line)) > 0:
                    continue

                include_name = ReSupport.include_dep(line)
                if len(include_name) > 0:
                    obj.deps.append(include_name[0])
                    continue

                include_name = ReSupport.include(line)
                if len(include_name) > 0:
                    obj.includes.append(include_name[0])
                    continue

                define_name = ReSupport.define(line)
                if len(define_name) > 0:
                    obj.defines.append(define_name[0])
                    continue

                obj.out += line
        return obj

    def __add__(self, other):
        out = self.out + other.out
        deps = self.deps + other.deps
        includes = self.includes + other.includes
        defines = self.defines + other.defines
        return SourceInfo.set_with(out, deps, includes, defines)


def file_list(dirname, ban_dir=[]):
    files = []
    if isinstance(ban_dir, str):
        ban_dir = [ban_dir]

    for name in os.listdir(dirname):
        full_path = os.path.join(dirname, name)
        if os.path.isfile(full_path):
            files.append(full_path)
        elif os.path.isdir(full_path) and name not in ban_dir:
            files += file_list(full_path)
    return files


def in_endswith(name, files):
    for f in files:
        if f.endswith(name):
            return f
    return None


def order_dep(deps, files, done):
    info = SourceInfo()
    for dep in deps:
        if in_endswith(dep, done) is None:
            path = in_endswith(dep.split('/')[-1], files)

            if path is not None:
                new = SourceInfo.read_file(path)
                dep_new = order_dep(new.deps, files, done)

                info += dep_new + new
                done.append(path)

    return info


def none_preproc(dirname):
    if not os.path.exists(dirname):
        return [], ''
    
    dep = []
    out = ''
    includes = ''
    for files in os.listdir(dirname):
        with open(os.path.join(dirname, files)) as f:
            lines = f.readlines()
        
        if lines[0].startswith('#ifndef') \
            and lines[1].startswith('#define') \
            and lines[-1].startswith('#endif'):
            lines = lines[2:-1]
        
        data = ''.join(lines)

        include_idx = data.find('// merge:np_include')
        if include_idx != -1:
            start_idx = include_idx + len('// merge:np_include')
            end_idx = data.find('// merge:end', include_idx)
            
            includes += data[start_idx:end_idx]
            data = data[end_idx + len('// merge:end'):]

        include_idx = data.find('// merge:include')
        if include_idx != -1:
            start_idx = include_idx + len('// merge:include')
            end_idx = data.find('// merge:end', include_idx)

            for line in data[start_idx:end_idx].split('\n'):
                res = ReSupport.include(line)
                if len(res) > 0:
                    dep += res
                    includes += line + '\n'

            data = data[end_idx + len('// merge:end'):]

        dep.append(files)
        out += data

    return dep, includes + out


def merge(dirname):
    done = []
    info = SourceInfo()
    files = file_list(dirname, 'platform')

    preproc_dep, info.out = \
        none_preproc(os.path.join(dirname, 'platform'))

    for full_path in files:
        if full_path not in done:
            source = SourceInfo.read_file(full_path)
            dep = order_dep(source.deps, files, done)

            info += dep + source
            done.append(full_path)

    return info, preproc_dep


def write_hpp(outfile, merged):
    info, preproc_dep = merged
    dep_check = lambda file: not any(file.endswith(dep) for dep in preproc_dep)

    idx = outfile.rfind('/')
    if idx > -1:
        outfile = outfile[idx+1:]
    guard_name = outfile.upper().replace('.', '_')

    unique_include = []

    for include in info.includes:
        if include not in unique_include and dep_check(include):
            unique_include.append(include)

    includes = '\n'.join(
        '#include <{}>'.format(x) for x in sorted(unique_include)) + '\n'
    defines = '\n'.join(
        '#define {}'.format(x) for x in info.defines) + '\n'

    out = Format.hpp.format(guard_name, includes, defines, info.out)
    out = Format.hpp_beutifier(out)

    with open(outfile, 'w') as f:
        f.write(out)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        outfile = sys.argv[1]
    else:
        outfile = './obfuscator.hpp'

    if len(sys.argv) > 2:
        dirname = sys.argv[2]
    else:
        dirname = './obfuscator/obfs'

    merged = merge(dirname)
    write_hpp(outfile, merged)
