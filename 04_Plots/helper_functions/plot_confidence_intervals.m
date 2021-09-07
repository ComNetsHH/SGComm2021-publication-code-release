function [ax] = plot_confidence_intervals(ax,xData,yData,experiments)
    global confidenceLevel;
    yData(isinf(yData)) = NaN;
    yDataCI = zeros(size(xData,2),2);
    for xID = 1:size(xData,2)
       yDataCI(xID,:) =  calculate_ci(yData(:,xID),confidenceLevel);
    end
    yDataMean = mean(yData,1,'omitnan');
    
    if(length(get(ax, 'Children'))==0)
        marker ='o';
    else
        marker ='d';
    end
    
    
    errorbar(ax,xData(1,:), yDataMean, yDataCI(:,1)', yDataCI(:,2)','MarkerSize',get(0,'defaultLineMarkerSize'),'MarkerFaceColor','auto','Marker',marker,'LineStyle','none','DisplayName',strcat(experiments(1).results.name," (N=",num2str(size(yData,1)),")"))
end
