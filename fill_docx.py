import zipfile
import xml.etree.ElementTree as ET
import os
import shutil
import copy

namespaces = {'w': 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}

def set_cell_text(cell, text):
    # Find cell properties (tcPr) if it exists
    tcPr = cell.find('w:tcPr', namespaces)
    
    # Remove all children of cell except tcPr
    for child in list(cell):
        if child != tcPr:
            cell.remove(child)
            
    # Split text by newlines and create w:p with line breaks
    p = ET.Element('{http://schemas.openxmlformats.org/wordprocessingml/2006/main}p')
    r = ET.Element('{http://schemas.openxmlformats.org/wordprocessingml/2006/main}r')
    
    lines = text.split('\n')
    for i, line in enumerate(lines):
        if i > 0:
            r.append(ET.Element('{http://schemas.openxmlformats.org/wordprocessingml/2006/main}br'))
        t = ET.Element('{http://schemas.openxmlformats.org/wordprocessingml/2006/main}t')
        t.text = line
        t.set('{http://www.w3.org/XML/1998/namespace}space', 'preserve')
        r.append(t)
        
    p.append(r)
    cell.append(p)

def main():
    docx_path = r"C:\Users\mansi\Desktop\3rd phase Project Progress Temp-GEHU.docx"
    extract_dir = r"c:\Users\mansi\Desktop\compilingTheory-master\scratch\docx_contents"
    
    # Clean extract_dir if exists
    if os.path.exists(extract_dir):
        shutil.rmtree(extract_dir)
    os.makedirs(extract_dir)
    
    # Unzip docx
    with zipfile.ZipFile(docx_path, 'r') as zip_ref:
        zip_ref.extractall(extract_dir)
        
    document_xml_path = os.path.join(extract_dir, 'word', 'document.xml')
    
    # Register namespaces to prevent namespace prefix issue (like ns0:)
    for prefix, uri in namespaces.items():
        ET.register_namespace(prefix, uri)
    # Also register other common docx namespaces just in case
    ET.register_namespace('r', 'http://schemas.openxmlformats.org/officeDocument/2006/relationships')
    ET.register_namespace('m', 'http://schemas.openxmlformats.org/officeDocument/2006/math')
    ET.register_namespace('v', 'urn:schemas-microsoft-com:vml')
    ET.register_namespace('wp', 'http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing')
    ET.register_namespace('w10', 'urn:schemas-microsoft-com:office:word')
    
    tree = ET.parse(document_xml_path)
    root = tree.getroot()
    
    tables = root.findall('.//w:tbl', namespaces)
    print(f"Found {len(tables)} tables to populate.")
    
    # Table 0: Project Title
    print("Populating Table 0: Project Title")
    cells_t0 = tables[0].findall('.//w:tc', namespaces)
    set_cell_text(cells_t0[0], "Interactive Compiler Visualizer (Lexer, LL(1)/LR(1) Parsers, & Regex-DFA Engine)")
    
    # Table 1: GitHub URL
    print("Populating Table 1: GitHub URL")
    cells_t1 = tables[1].findall('.//w:tc', namespaces)
    set_cell_text(cells_t1[0], "https://github.com/blackhat289/CodeScope-Compiler-Visualizer")
    
    # Table 2: Student/Team Information
    print("Populating Table 2: Team Info")
    # Row 0: Team Name -> Col 1
    t2_rows = tables[2].findall('.//w:tr', namespaces)
    set_cell_text(t2_rows[0].findall('.//w:tc', namespaces)[1], "Tokens")
    # Row 1-4: Team members -> Col 1
    set_cell_text(t2_rows[1].findall('.//w:tc', namespaces)[1], "Mansi (Student ID: 23012995, Roll No: 2319079, Email: mansinegi739@gmail.com)")
    set_cell_text(t2_rows[2].findall('.//w:tc', namespaces)[1], "Dev Vyas Arora (Student ID: 23011798, Roll No: 2318712, Email: aroradevvyas@gmail.com)")
    set_cell_text(t2_rows[3].findall('.//w:tc', namespaces)[1], "Aarush Mandoliya (Student ID: 230114980, Roll No: 2318115, Email: aarushmandoliya@gmail.com)")
    set_cell_text(t2_rows[4].findall('.//w:tc', namespaces)[1], "Karan Singh Chauhan (Student ID: 23011307, Roll No: 2318949, Email: K03073300@gmail.com)")
    
    # Table 3: Project Abstract
    print("Populating Table 3: Abstract")
    cells_t3 = tables[3].findall('.//w:tc', namespaces)
    set_cell_text(cells_t3[0], (
        "This project presents an interactive web-based Compiler Visualizer designed for educational purposes "
        "to demonstrate the core stages of compiler design. It consists of four integrated modules: a Lexical "
        "Analyzer (Lexer), an LL(1) Parser, an LR(1) Parser, and a Regex-to-DFA Converter. The Lexer tokenizes "
        "C-like source code into keywords, identifiers, operators, and constants. The LL(1) Parser computes "
        "FIRST/FOLLOW sets and performs top-down parsing. The LR(1) Parser generates DFA state transitions, Action/Goto "
        "tables, and runs a bottom-up shift-reduce parse. The Regex-DFA engine translates regular expressions into NFAs "
        "(via Thompson's construction), then to DFAs (via Subset construction), minimizes them using Hopcroft's algorithm, "
        "and visualizes string matching. By rendering visual step-by-step simulations, the toolkit bridges the gap "
        "between compiler theory and practical programming."
    ))
    
    # Table 4: Updated Project Approach and Architecture
    print("Populating Table 4: Approach & Architecture")
    cells_t4 = tables[4].findall('.//w:tc', namespaces)
    set_cell_text(cells_t4[0], (
        "Our project uses a multi-tier architecture to deliver high-performance parsing and dynamic web visualizations:\n"
        "1. Core Engines (C++ Backend): Written in standard C++ for speed and efficiency. Handles tokenization, "
        "parsing table construction (LL(1) FIRST/FOLLOW, LR(1) closure/goto), DFA minimization, and simulation logic.\n"
        "2. Integration API (PHP Layer): Serves as the communication link. Receives source code or grammar inputs "
        "from the frontend, executes the compiled C++ modules, captures stdout, and returns structured JSON responses.\n"
        "3. Interactive UI (HTML5/CSS3/JS Frontend): Built with Semantic UI and jQuery. Features a dual-panel layout "
        "(inputs on the left, visual simulations/tables on the right) and integrates Graphviz (via Viz.js) for live DFA rendering.\n"
        "4. Libraries: Standard C++ STL for data structures; jQuery and Semantic UI for frontend components; "
        "Viz.js and Treant.js for rendering trees, DFA transitions, and parse status graphs."
    ))
    
    # Table 5: Tasks Completed (Requires row cloning)
    print("Populating Table 5: Tasks Completed")
    tasks_completed = [
        ("Designing and implementing the C++ Lexical Analyzer using Trie-based keyword lookup", "Mansi"),
        ("Designing LL(1) Parser and predictive parsing simulation in C++", "Dev Vyas Arora"),
        ("Implementing LR(1) canonical item set generation, GOTO/ACTION table builder, and shift-reduce parsing", "Aarush Mandoliya"),
        ("Developing the Regex-to-DFA engine (Thompson, Subset Construction, Hopcroft Minimization, and String Matcher)", "Karan Singh Chauhan"),
        ("Building the PHP backend API to connect C++ execution with JSON messaging", "Mansi, Karan Singh Chauhan"),
        ("Designing the Web UI frontend using Semantic UI, jQuery, and Integrating Viz.js/Graphviz for live DFA rendering", "Aarush Mandoliya, Dev Vyas Arora")
    ]
    
    t5_rows = tables[5].findall('.//w:tr', namespaces)
    template_row = t5_rows[1]
    
    # Fill the first row (the template row itself)
    cells_tmpl = template_row.findall('.//w:tc', namespaces)
    set_cell_text(cells_tmpl[0], tasks_completed[0][0])
    set_cell_text(cells_tmpl[1], tasks_completed[0][1])
    
    # Clone and append other rows
    for task, member in tasks_completed[1:]:
        new_row = copy.deepcopy(template_row)
        new_cells = new_row.findall('.//w:tc', namespaces)
        set_cell_text(new_cells[0], task)
        set_cell_text(new_cells[1], member)
        tables[5].append(new_row)
        
    # Table 6: Challenges/Roadblocks
    print("Populating Table 6: Challenges")
    cells_t6 = tables[6].findall('.//w:tc', namespaces)
    set_cell_text(cells_t6[0], (
        "1. Graphviz Integration: Generating and rendering complex DFA graphs dynamically in the browser was slow; "
        "resolved by implementing client-side Graphviz rendering via Viz.js.\n"
        "2. LR(1) Canonical Collection: Computing lookahead sets for LR(1) items during closure operations was highly "
        "complex and prone to infinite loops; resolved by implementing a rigorous FIRST set lookahead calculation.\n"
        "3. State Synchronization: Ensuring the step-by-step visual simulation (stack states, input tape, and current action) "
        "correctly mirrors the C++ execution trace; solved by exporting detailed step-by-step state arrays in the JSON API.\n"
        "4. Epsilon Transition Handling: Managing empty transitions (epsilon / '@') in both LL(1) empty productions "
        "and NFA-to-DFA conversion required careful verification of edge cases."
    ))
    
    # Table 7: Tasks Pending (Requires row cloning)
    print("Populating Table 7: Tasks Pending")
    tasks_pending = [
        "Adding more comprehensive syntax error detection and friendly error reporting in LL(1) and LR(1) parsers.",
        "Support for semantic check visualizations (e.g., type matching, scope validation).",
        "Extended testing with complex grammars (e.g., standard ANSI C subset grammars).",
        "Creating user guide documentation and adding pre-loaded grammar examples in the UI."
    ]
    t7_rows = tables[7].findall('.//w:tr', namespaces)
    template_row_t7 = t7_rows[1]
    
    # Fill first pending task
    set_cell_text(template_row_t7.findall('.//w:tc', namespaces)[0], tasks_pending[0])
    
    # Clone and append others
    for task in tasks_pending[1:]:
        new_row = copy.deepcopy(template_row_t7)
        new_cells = new_row.findall('.//w:tc', namespaces)
        set_cell_text(new_cells[0], task)
        tables[7].append(new_row)
        
    # Table 8: Project Outcome/Deliverables
    print("Populating Table 8: Deliverables")
    cells_t8 = tables[8].findall('.//w:tc', namespaces)
    set_cell_text(cells_t8[0], (
        "1. Fully Functional Web Visualizer: Interactive frontend panels for Lexer, LL(1) parsing, LR(1) parsing, and Regex-DFA conversion.\n"
        "2. C++ Compiler Modules: Clean, compiled backend executables for tokenization and parsing logic.\n"
        "3. Dynamic DFA Graph Renderings: Real-time visual DFA state transition graphs rendered in-browser.\n"
        "4. Parsing Table Generation: Auto-constructed LL(1) predictive parsing tables and LR(1) ACTION/GOTO tables.\n"
        "5. Trace Simulations: Step-by-step stack transition records showing matching, shift, and reduce operations."
    ))
    
    # Table 9: Progress Overview
    print("Populating Table 9: Progress Overview")
    cells_t9 = tables[9].findall('.//w:tc', namespaces)
    set_cell_text(cells_t9[0], (
        "About 90% of the project is completed.\n"
        "- Completed: Backend C++ engines for all four modules (Lexer, LL(1), LR(1), Regex-DFA), PHP API wrapper, and the core web dashboard UI with live Graphviz rendering.\n"
        "- In Progress: Polishing UI responsiveness, adding pre-configured grammar templates, and enhancing syntax error highlighting.\n"
        "- Left to do: Adding comprehensive error-recovery strategies and final system documentation."
    ))
    
    # Table 10: Testing and Validation Status (Requires row cloning)
    print("Populating Table 10: Testing Status")
    testing_status = [
        ("Lexical Analyzer Test", "Pass", "Successfully tokenizes sample C code and generates correct JSON representations of token types and positions."),
        ("LL(1) Parser Test", "Pass", "Computes FIRST and FOLLOW sets, builds predictive parsing tables, and parses arithmetic expression grammars correctly."),
        ("LR(1) Parser Test", "Pass", "Generates canonical collections, ACTION/GOTO tables, and correctly visualizes the LR(1) DFA with shift/reduce actions."),
        ("Regex to DFA Test", "Pass", "Correctly performs Thompson's NFA construction, subset construction, Hopcroft's minimization, and string matching verification.")
    ]
    t10_rows = tables[10].findall('.//w:tr', namespaces)
    template_row_t10 = t10_rows[1]
    
    # Fill first test row
    cells_t10 = template_row_t10.findall('.//w:tc', namespaces)
    set_cell_text(cells_t10[0], testing_status[0][0])
    set_cell_text(cells_t10[1], testing_status[0][1])
    set_cell_text(cells_t10[2], testing_status[0][2])
    
    # Clone and append others
    for t_type, t_status, t_notes in testing_status[1:]:
        new_row = copy.deepcopy(template_row_t10)
        new_cells = new_row.findall('.//w:tc', namespaces)
        set_cell_text(new_cells[0], t_type)
        set_cell_text(new_cells[1], t_status)
        set_cell_text(new_cells[2], t_notes)
        tables[10].append(new_row)
        
    # Table 11: Deliverables Progress
    print("Populating Table 11: Deliverables Progress")
    cells_t11 = tables[11].findall('.//w:tc', namespaces)
    set_cell_text(cells_t11[0], (
        "1. Lexical Analyzer: Completed. High-speed tokenization with symbol tables is fully functional.\n"
        "2. LL(1) Parser: Completed. FIRST/FOLLOW sets and stack-based parsing are operational.\n"
        "3. LR(1) Parser: Completed. Item sets, ACTION/GOTO, and DFA graph visualization are operational.\n"
        "4. Regex to DFA: Completed. Thompson's NFA, subset construction, and minimization are operational.\n"
        "5. Web Frontend UI: Completed. Clean web interface integrating all modules.\n"
        "6. Error recovery: In Progress. Simple error states are reported, but advanced recovery is pending."
    ))
    
    # Save the modified document.xml
    tree.write(document_xml_path, encoding='utf-8', xml_declaration=True)
    print("Saved modified document.xml.")
    
    # Repack zip to the original docx path
    if os.path.exists(docx_path):
        os.remove(docx_path)
        
    with zipfile.ZipFile(docx_path, 'w', zipfile.ZIP_DEFLATED) as zip_out:
        for root_dir, dirs, files in os.walk(extract_dir):
            for file in files:
                full_path = os.path.join(root_dir, file)
                rel_path = os.path.relpath(full_path, extract_dir)
                zip_out.write(full_path, rel_path)
                
    print(f"Success! Document filled and saved to {docx_path}")
    
    # Clean up temporary contents directory
    shutil.rmtree(extract_dir)

if __name__ == "__main__":
    main()
