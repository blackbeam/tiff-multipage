{
    "targets": [
        {
            "target_name": "tiff-multipage",
            "sources": [
                "src/module.cc",
                "src/sync.cc",
                "src/async.cc",
                "src/tiff_multipage.cc"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                ".",
            ],
            "conditions": [
                ['OS=="win"', {
                    'include_dirs': ['.', '../..', '../../deps/libtiff'],
                    "libraries": [
                        'msvcrt.lib',
                        'libcmt.lib',
                        "libtiff.lib",
                    ],
                    "configurations": {
                        'Release': { 'msvc_settings': { 'VCCLCompilerTool': { 'RuntimeLibrary': '2' } } }
                    },
                    "msvs_settings": { 'VCLinkerTool': { "AdditionalLibraryDirectories": [".", '../..', "<(module_root_dir)", '../../deps/libtiff'] } }
                }],
                ['OS!="win"', {
                    'libraries': ["-ltiff", "<!@(pkg-config --libs libtiff-4)"],
                    'include_dirs': ["<!@(pkg-config libtiff-4 --cflags-only-I | sed s/-I//g)"]
                }],
            ]
        }
    ]
}
