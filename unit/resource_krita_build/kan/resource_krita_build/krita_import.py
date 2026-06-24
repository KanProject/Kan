from krita import *
from pathlib import Path

def __main__(args):
    app = Krita.instance()
    app.setBatchmode(True)

    document_path = args[0]
    export_directory = args[1]
    scale_factor = float(args[2])
    file_prefix = Path(document_path).stem
    document = app.openDocument(document_path)

    if (round(document.width() * scale_factor) != document.width()):
        document.scaleImage(round(document.width() * scale_factor),
                            round(document.height() * scale_factor),
                            round(document.xRes()),
                            round(document.yRes()),
                            "Bilinear")

    pngOptions = InfoObject()
    pngOptions.setProperty('alpha', True)
    pngOptions.setProperty('compression', 1)
    pngOptions.setProperty('forceSRGB', True)
    pngOptions.setProperty('indexed', False)
    pngOptions.setProperty('interlaced', False)
    pngOptions.setProperty('saveSRGBProfile', True)
    pngOptions.setProperty('transparencyFillcolor', QColor(0, 0, 0, 0))

    with open(export_directory + "/" + "header.rd", 'w') as header:
        header.write("//! kan_resource_krita_header_t\n\n")

        # Export all layers that are marked for export.
        for layer in document.rootNode().findChildNodes("::", True, True):
            header.write("+entries {\n")

            whole_name = layer.name()
            meta_separator = whole_name.find("::")
            item_name = whole_name[:meta_separator].strip()
            
            # Inherit prefixes from parent layers.
            parent_layer = layer.parentNode()
            
            while parent_layer:
                if len(parent_layer.name()) > 0 and parent_layer.name()[0] == '@':
                    parent_prefix = parent_layer.name()[1:].strip()
                    item_name = (parent_prefix + "_" + item_name) if item_name else parent_prefix
                parent_layer = parent_layer.parentNode()
                
            # Inherit file prefix.
            item_name = (file_prefix + "_" + item_name) if item_name else file_prefix

            filter = ""
            locale = ""
            meta = whole_name[(meta_separator + 2):].strip().split()

            for meta_item in meta:
                if meta_item[0] == 'F': filter = meta_item[1:]
                if meta_item[0] == 'L': locale = meta_item[1:]

            file_name = item_name + ("_" if locale else "") + locale + ".png"
            header.write("    name = \"%s\"\n" % item_name)
            header.write("    file = \"%s\"\n" % file_name)

            if locale: header.write("    locale = \"%s\"\n" % locale)
            if filter: header.write("    filter = \"%s\"\n" % filter)
            header.write("}\n\n")

            layer.save(export_directory + "/" + file_name,
                       document.xRes(),
                       document.yRes(),
                       pngOptions,
                       document.bounds())

    document.close()
    # Kritarunner always exits with 0, making it impossible to report errors normally.
    # Therefore, we create "success.txt" empty file to let the build system know that execution was successful.
    with open(export_directory + "/" + "success.txt", 'x') as file:
        pass
