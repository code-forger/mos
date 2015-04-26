cd ~/Documents/OSProject/FReport
inkscape -D -z --file=images/Process_Table_Diagram.svg --export-pdf=Process_Table_Diagram.pdf --export-latex
pdflatex -interaction="batchmode" report.tex
bibtex report.aux
pdflatex -interaction="batchmode" report.tex