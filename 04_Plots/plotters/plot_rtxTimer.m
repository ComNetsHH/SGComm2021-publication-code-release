function [ax] = plot_rtxTimer(ax,xData,yData)
    global confidenceLevel;

    yData(isinf(yData)) = NaN;
    yDataCI = zeros(size(xData,2),size(yData,3),2);
    for xID = 1:size(xData,2)
        for rtxID = 1:size(yData,3)
            yDataCI(xID,rtxID,:) =  calculate_ci(yData(:,xID,rtxID),confidenceLevel);
        end
    end
    yDataMean = mean(yData,1,'omitnan');
    yDataCI(isnan(yDataCI)) = 0;
    
    colorOrder = get(gca, 'ColorOrder');
    if(length(get(ax, 'Children'))>1)
        color =colorOrder(2,:);
        mSize = 20;
    else
        color = colorOrder(1,:);
        mSize = 10;
    end
    
    marker = ["--x","--o","--s","--d"];
    for rtxID = 1:size(yData,3)
        %boundedline(ax,xData, yDataMean(:,:,rtxID), yDataCI(:,rtxID,1)',...
        %               xData, yDataMean(:,:,rtxID), -yDataCI(:,rtxID,2)',...
        %               'cmap', color);
        %plot(ax,xData, yDataMean(:,:,rtxID),marker(rtxID),'Color',color);
        if(rtxID > 1)
            h = scatter(ax,xData, yDataMean(:,:,rtxID),mSize,'MarkerEdgeColor',color,'Marker','.');
            set(get(get(h,'Annotation'),'LegendInformation'),'IconDisplayStyle','off');
        else
            h = scatter(ax,xData, yDataMean(:,:,rtxID),mSize,'MarkerEdgeColor',color,'Marker','.');
        end

    end
    

end
