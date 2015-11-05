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
                "<!@(pkg-config libtiff-4 --cflags-only-I | sed s/-I//g)"
            ],
            "libraries": [
                "-ltiff",
                "<!@(pkg-config --libs libtiff-4)"
            ]
        }
    ]
}
