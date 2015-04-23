cd ~/Documents/OSProject/FReport
pdflatex -interaction="batchmode" report.tex
bibtex report.aux
pdflatex -interaction="batchmode" report.tex