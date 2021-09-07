%% Static Config
rtxAxis = [10:10:400];
seqAxis = [1:58];

%% Data Plot
experiments =  load_experiments(path_to_processed_results,aoi_matrix_experiment_name);
[H,ax] = plot_pcolor_matrix(experiments,seqAxis,rtxAxis);

%% Formating


ylabel({'$t_{rtx}$ [ms]'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
xlabel({'Parallel Streams'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')

h =colorbar;
ax.MeshStyle = "none";
h.Label.String = "Avg. AoI [s]";
h.Label.Interpreter = 'latex';
h.Label.FontSize = 9;
h.TickLabels = [h.TickLabels(1:end-1); ">14"];
h.Ticks = [h.Ticks(1:end-1) 14];
h.Label.Position = [2.0853    8.5107         0];

%% Export
if(export_figures)
    export_figure(H,"aoi_matrix",path_to_export_figures);
end