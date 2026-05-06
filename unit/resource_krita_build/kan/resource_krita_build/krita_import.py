from krita import *
from pathlib import Path

SRGB_PROFILE = "sRGB-elle-V2-srgbtrc.icc"

def __main__(args):
    app = Krita.instance()
    document_path = args[0]
    export_directory = args[1]
    scale_factor = float(args[2])
    prefix = Path(document_path).stem
    document = app.openDocument(document_path)
    bounds = document.bounds()

    x = bounds.x()
    y = bounds.y()
    w = bounds.width()
    h = bounds.height()
    scaled_w = round(float(w) * scale_factor)

    with open(export_directory + "/" + "header.rd", 'w') as header:
        header.write("//! kan_resource_krita_header_t\n\n")

        # Export all layers that are marked for export.
        for layer in document.rootNode().findChildNodes("::", True, True):
            header.write("+entries {\n")

            whole_name = layer.name()
            meta_separator = whole_name.find("::")
            item_name = whole_name[:meta_separator].strip()
            item_name = (prefix + "_" + item_name) if item_name else prefix

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

            if (layer.colorModel() == "RGBA" and
                    layer.colorDepth() == "U8" and
                    layer.colorProfile().lower() == SRGB_PROFILE.lower()):

                pixel_data = layer.projectionPixelData(x, y, w, h).data()
            else:
                temp_node = layer.duplicate()
                temp_node.setColorSpace("RGBA", "U8", SRGB_PROFILE)
                pixel_data = temp_node.projectionPixelData(x, y, w, h).data()

            image = QImage(pixel_data, w, h, QImage.Format.Format_ARGB32)
            if scaled_w != w:
                image = image.scaledToWidth(scaled_w, Qt.TransformationMode.SmoothTransformation)
            image.save(export_directory + "/" + file_name)

    document.close()
    # Kritarunner always exits with 0, making it impossible to report errors normally.
    # Therefore, we create "success.txt" empty file to let the build system know that execution was successful.
    with open(export_directory + "/" + "success.txt", 'x') as file:
        pass
