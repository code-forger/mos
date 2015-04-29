cd ~/Documents/OSProject/FReport
inkscape -D -z --file=images/Process_Table_Diagram.svg --export-pdf=Process_Table_Diagram.pdf --export-latex
echo -e "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" | pdflatex report.tex
bibtex report.aux
echo -e "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" | pdflatex report.tex