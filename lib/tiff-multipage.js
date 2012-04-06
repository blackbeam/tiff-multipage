try {
    module.exports = require('../build/Release/tiff-multipage').TIFFFile;
} catch (e) {
    module.exports = require('../build/default/tiff-multipage').TIFFFile;
}