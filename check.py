import glob
import sys
import clang.cindex
from clang.cindex import CursorKind, TokenKind


def extract_function(cursor):
    filename = cursor.location.file.name
    
    with open(filename, 'r') as f:
        contents = f.read()
    
    return contents[cursor.extent.start.offset: cursor.extent.end.offset]

def check_file(filename, verbose):
    with open(filename, 'r') as f:
        source = f.read()

    idx = clang.cindex.Index.create()
    tu = idx.parse(filename, args=['-std=c++11'],
                   unsaved_files=[(filename, source)],  options=0)

    for c in tu.cursor.walk_preorder():
        first_token = next(c.get_tokens(), None)

        if first_token is None:
            continue

        location = first_token.location

        if str(location.file) != filename:
            continue

        if c.kind == CursorKind.FUNCTION_DECL:
            func_size = len(extract_function(c))
            count = 0

            for token in c.get_tokens():
                if token.kind == TokenKind.COMMENT:
                    continue

                count += len(token.spelling)

            if verbose:
                print(' \u2022', c.spelling, '->', count, f'\033[91m({func_size})\033[0m' if func_size >= 555 else f'\033[90m({func_size})\033[0m')

            if count >= 555:
                exit(
                    f'\033[91merror:\033[0m {location.file}:{location.line}:{location.column}: function \033[4m{c.spelling}\033[0m has more than 555 characters')


def main():
    verbose = '-v' in sys.argv or '--verbose' in sys.argv
    for filename in glob.glob('src/*.cc', recursive=True):
        if verbose:
            print('-- Checking', f'\033[4m{filename}\033[0m')

        check_file(filename, verbose)


if __name__ == '__main__':
    main()
