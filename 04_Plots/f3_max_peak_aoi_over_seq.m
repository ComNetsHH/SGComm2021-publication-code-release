%% Static Config
fig = figure('Units','points','Position',[0 0 12*21 12*21],'PaperPositionMode','auto');
subplot(2,1,1)
hold on;
marker = 'x';
colororder(publicationColors)



%% Data Plot
field_name = 'peakAvgAoI';
for experiment_name_id = 1:size(peak_aoi_over_seq_experiment_names,1)
    experiments =  load_experiments(path_to_processed_results,peak_aoi_over_seq_experiment_names(experiment_name_id));
    plot_best_per_stream(fig,experiments,field_name);
end

% colororder(publicationColors)
% %% Data Plot
% field_name = 'avgAoI';
% for experiment_name_id = 1:size(peak_aoi_over_seq_experiment_names,1)
%     experiments =  load_experiments(path_to_processed_results,peak_aoi_over_seq_experiment_names(experiment_name_id));
%     plot_best_per_stream(fig,experiments,field_name);
% end

% 
% ylabel({'Peak AoI [s] / Avg. AoI [s]'},...
%     'FontUnits','points',...
%     'interpreter','latex',...
%     'FontName','Times')
ylabel({'Peak-to-Avg AoI'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
 xlabel({'Parallel Streams'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
       

%ylabel("Age of Information [ms]");
ylims =ylim;

lgd = legend(legendNames,'Location', 'NorthOutside');
%legend boxoff;
lgd.NumColumns = size(peak_aoi_over_seq_experiment_names,1);
lgd.ItemTokenSize = [10,10];
box on



%% Export
if(export_figures)
    export_figure(fig,"peak_aoi_over_seq",path_to_export_figures);
end
