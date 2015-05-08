cd ~/Documents/OSProject/FReport

dia -e TMPFILE -t eps images/Events_Structure.dia
ps2pdf -dEPSCrop TMPFILE images/Events_Structure.pdf

dia -e TMPFILE -t eps images/Caching_Speedup.dia
ps2pdf -dEPSCrop TMPFILE images/Caching_Speedup.pdf

dia -e TMPFILE -t eps images/Caching_Table.dia
ps2pdf -dEPSCrop TMPFILE images/Caching_Table.pdf

dia -e TMPFILE -t eps images/Heap_Manager.dia
ps2pdf -dEPSCrop TMPFILE images/Heap_Manager.pdf

dia -e TMPFILE -t eps images/Kernel_Moduels.dia
ps2pdf -dEPSCrop TMPFILE images/Kernel_Moduels.pdf

dia -e TMPFILE -t eps images/List_Speedup.dia
ps2pdf -dEPSCrop TMPFILE images/List_Speedup.pdf

dia -e TMPFILE -t eps images/Pipe_Manager.dia
ps2pdf -dEPSCrop TMPFILE images/Pipe_Manager.pdf

dia -e TMPFILE -t eps images/Process_Entry_Structure.dia
ps2pdf -dEPSCrop TMPFILE images/Process_Entry_Structure.pdf

dia -e TMPFILE -t eps images/Scheduler_Structure.dia
ps2pdf -dEPSCrop TMPFILE images/Scheduler_Structure.pdf

pdflatex --interaction="batchmode" report.tex
bibtex report.aux
pdflatex --interaction="batchmode" report.tex

rm TMPFILE