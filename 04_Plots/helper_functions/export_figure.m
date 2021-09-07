function [] = export_figure(H,figure_name,path_to_export)
mkdir(path_to_export);
print('-depsc2','-r2400',strcat(path_to_export,'/',figure_name,'plotfile.eps')) % Print to file
end

