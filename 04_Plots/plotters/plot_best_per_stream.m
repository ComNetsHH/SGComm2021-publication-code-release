function [H] = plot_best_per_stream(H,experiments,field_name)

global numRuns;

if(isempty(H))
    H = figure;
    axes(H);
end

maxSeq = 0;
maxRng = 0;
numBBNodes = 0;
for experimentID = 1:size(experiments,2)
    
    if(isfield(experiments(experimentID).results,'rngID'))
        experiments(experimentID).results.rngID = str2double(experiments(experimentID).results.rngID);
    else
        experiments(experimentID).results.rngID = 1;
    end
    
    if(experiments(experimentID).results.numSeq>maxSeq)
        maxSeq=experiments(experimentID).results.numSeq;
    end
    if(experiments(experimentID).results.rngID>maxRng)
        maxRng = experiments(experimentID).results.rngID;
    end
    if(isfield(experiments(1).results,'rtxMean'))
        if(size(experiments(1).results.rtxMean,2)>numBBNodes)
            numBBNodes = size(experiments(1).results.rtxMean,2);
        end
    else
        numBBNodes=1;
    end
end

switch(field_name)
    case 'avgAoI'
        xData = repmat([1:maxSeq],maxRng,1);
        yData = Inf(maxRng,maxSeq);

        for experimentID = 1:size(experiments,2)
            if(isfield(experiments(experimentID).results,'avgAoI'))
                if(yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
                    > experiments(experimentID).results.avgAoI)
                    yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
                end
            end
        end
        % Squash Inf rows if missing data
        yData(all(isinf(yData),2),:)=[];
        % Only select first numRuns entries
        if(size(yData,1)>numRuns)
            yData = yData(1:numRuns,:);
            xData = xData(1:numRuns,:);
        end
        plot_confidence_intervals(H.CurrentAxes,xData,yData./1000,experiments);
        
    case 'peakAvgAoI'
        xData = repmat([1:maxSeq],maxRng,1);
        yData = Inf(maxRng,maxSeq);
        yData2 = Inf(maxRng,maxSeq);
        for experimentID = 1:size(experiments,2)
            if(yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
                    > experiments(experimentID).results.peakAvgAoI)
                yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = max(experiments(experimentID).results.peakAvgAoI);
                yData2(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
            end
        end
        % Squash Inf rows if missing data
        yData(all(isinf(yData),2),:)=[];
        yData2(all(isinf(yData2),2),:)=[];

        % Only select first numRuns entries
        if(size(yData,1)>numRuns)
            yData = yData(1:numRuns,:);
            yData2 = yData2(1:numRuns,:);
            xData = xData(1:numRuns,:);
        end
        plot_confidence_intervals_peak(H.CurrentAxes,xData,(yData./yData2),experiments);
        %plot_confidence_intervals_peak(H.CurrentAxes,xData,yData./1000,experiments);
        
    case 'rtxMean'
        xData = 1:maxSeq;
        yData = Inf(maxRng,maxSeq,numBBNodes);
        yDataAvg = Inf(maxRng,maxSeq);

        for experimentID = 1:size(experiments,2)
            if(isfield(experiments(experimentID).results,'rtxMean'))
                if(experiments(experimentID).results.rtxMean(1)~=0)
                    yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq,:) = experiments(experimentID).results.rtxMean;
                else
                    if(yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
                        > experiments(experimentID).results.avgAoI)
                        yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
                        yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq,1) = experiments(experimentID).results.rtxTimer;
                    end
                end
            else
                if(yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
                    > experiments(experimentID).results.avgAoI)
                    yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
                    yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq,1) = experiments(experimentID).results.rtxTimer;
                end
            end
        end
        % Squash Inf rows if missing data
        yData(all(isinf(yData),2),:)=[];
        % Only select first numRuns entries
        if(size(yData,1)>numRuns)
            yData = yData(1:numRuns,:);
            xData = xData(1:numRuns,:);
        end
        plot_rtxTimer(H.CurrentAxes,xData,yData);
        
        
    case 'etx'
        xData = repmat([1:maxSeq],maxRng,1);
        yData = Inf(maxRng,maxSeq);
        yDataAvg = Inf(maxRng,maxSeq);
        for experimentID = 1:size(experiments,2)
            if(isfield(experiments(experimentID).results,'rtxMean'))
                if(experiments(experimentID).results.rtxMean(1)~=0)
                    yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq,:) = mean(experiments(experimentID).results.etx)./72;
                else
                    if(yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
                        > experiments(experimentID).results.avgAoI)
                        yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
                        yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq,1) = mean(experiments(experimentID).results.etx)./72;
                    end
                end
            else
                if(yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
                    > experiments(experimentID).results.avgAoI)
                    yDataAvg(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
                    yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq,1) = mean(experiments(experimentID).results.etx)./72;
                end
            end
        end
        % Squash Inf rows if missing data
        yData(all(isinf(yData),2),:)=[];
        % Only select first numRuns entries
        if(size(yData,1)>numRuns)
            yData = yData(1:numRuns,:);
            xData = xData(1:numRuns,:);
        end
        plot_confidence_intervals(H.CurrentAxes,xData,yData,experiments);
        
    case 'rtx'
        xData = repmat([1:maxSeq],maxRng,1);
        yData = Inf(maxRng,maxSeq);

        for experimentID = 1:size(experiments,2)
            if(isfield(experiments(experimentID).results,'rtxShare'))
                yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = sum(experiments(experimentID).results.rtxShare)/nnz(experiments(experimentID).results.txTime)/mean(experiments(experimentID).results.etx);
            end
        end
        % Squash Inf rows if missing data
        yData(all(isinf(yData),2),:)=[];
        % Only select first numRuns entries
        if(size(yData,1)>numRuns)
            yData = yData(1:numRuns,:);
            xData = xData(1:numRuns,:);
        end
        plot_confidence_intervals(H.CurrentAxes,xData,yData,experiments);
        
  end
end

