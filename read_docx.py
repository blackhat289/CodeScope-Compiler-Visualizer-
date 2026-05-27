import zipfile
import xml.etree.ElementTree as ET
import sys

namespaces = {'w': 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}

def read_docx(file_path):
    with zipfile.ZipFile(file_path) as docx:
        tree = ET.parse(docx.open('word/document.xml'))
        root = tree.getroot()
        
        # We want to print in order
        body = root.find('w:body', namespaces)
        if body is None:
            print("No body found")
            return
            
        for child in body:
            # check tag
            tag = child.tag.split('}')[-1]
            if tag == 'p':
                text = "".join([node.text for node in child.findall('.//w:t', namespaces) if node.text])
                if text.strip():
                    print(f"P: {text}")
            elif tag == 'tbl':
                print("TABLE:")
                for row in child.findall('.//w:tr', namespaces):
                    row_cells = []
                    for cell in row.findall('.//w:tc', namespaces):
                        cell_text = " ".join([node.text for node in cell.findall('.//w:t', namespaces) if node.text])
                        row_cells.append(cell_text.strip())
                    print(f"  ROW: {row_cells}")

if __name__ == '__main__':
    read_docx(sys.argv[1])
