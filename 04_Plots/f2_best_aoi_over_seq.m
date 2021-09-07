%% Static Config
fig = figure('Units','points','Position',[0 0 12*21 12*21],'PaperPositionMode','auto');
subplot(2,1,1)
colororder(publicationColors)

hold on;
marker = 'x';

%% Data Plot
field_name = 'avgAoI';
for experiment_name_id = 1:size(aoi_over_seq_experiment_names,1)
    experiments =  load_experiments(path_to_processed_results,aoi_over_seq_experiment_names(experiment_name_id));
    plot_best_per_stream(fig,experiments,field_name);
end
ax = fig.Children;
ax.Children(1).Marker="s";

ylabel({'Avg. AoI [s]'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
xlabel({'Parallel Streams'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
       

%ylabel("Age of Information [ms]");
ylims =ylim;
ax = fig.Children;
ax.Children(1).Marker="s";


lgd = legend(legendNames,'Location', 'NorthOutside');
%legend boxoff;
lgd.NumColumns = size(aoi_over_seq_experiment_names,1);
lgd.ItemTokenSize = [10,10];
box on

%% Export
if(export_figures)
    export_figure(fig,savename,path_to_export_figures);
end