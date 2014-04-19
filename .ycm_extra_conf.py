flags = [
        '-Wall',
        '-Wextra',
        '-Werror',
        '-std=c99',
        '-x',
        'c',
        '-isystem',
        '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/c++/v1',
        '-isystem',
        '/usr/local/include',
        '-isystem',
        '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/5.0/include',
        '-isystem',
        '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include',
        '-isystem',
        '/usr/include',
        '-isystem',
        '/System/Library/Frameworks',
        '-isystem',
        '/Library/Frameworks',
        '-I',
        '.',
]

def FlagsForFile( filename, **kwargs ):
    return {
        'flags': flags,
        'do_cache': True
    }

