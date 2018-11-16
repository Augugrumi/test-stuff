#!/usr/bin/env Rscript

print_boxplot = function(data, path, name) {
    png(filename=sprintf("%s/%sGraph.png", path, name))
    time = list()
    for (i in 1:10) {
        time[[i]] = range01(data[data$Elements == i, 1])
    }
#     names = structure(list(range01(data[,1]), range01(data[,2])), .Names = c("Virtual Box", "Docker"), class = "data.frame")
    boxplot(time)
    dev.off()
}

print_pie = function(data, path, name) {
     png(filename=sprintf("%s/%sGraph.png", path, name))
     pie(
        c(sum(data[,1]), sum(data[,2])),
        col=c("darkgreen", "darkred"),
        labels = c("Virtual Box", "Docker"),
        main = "Docker vs VM start up times")
     dev.off()
}

print_barplot = function(data, path, name) {
    png(filename=sprintf("%s/%sGraph.png", path, name))
    time = list()
    for (i in 1:10) {
        time[[i]] = sum(data[data$Elements == i, 1])
    }
    barplot(
        unlist(time),
        main="SFC start up times",
        col=c("darkgreen", "darkred", "darkmagenta", "darkorange", "darkslateblue", "darkslategray", "gold", "dimgray", "darksalmon", "deepskyblue"),
        ylim=c(0, round((max(unlist(time)) / 500)) + 1) * 500,
        names.arg=c("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"))
#         cex.lab=2,
#         cex.axis=2,
#         cex.names=2,
#         cex.main=2)
    mtext(side=1, line=3, text="SFC length")
    mtext(side=2, line=3, text="seconds")
    dev.off()
}

range01 = function(x) {
    (x-min(x))/(max(x)-min(x))
}

append = function(x, y) {
    paste(x, y, sep='')
}

args = commandArgs(trailingOnly=TRUE)
if (length(args) < 2) {
    stop("At least three arguments must be supplied (path to the data, path where to save the graphs and name prefix).n", call.=FALSE)
} else {
    data = read.csv(args[1], sep=",")
#     print_pie(data, args[2], append(args[3], 'Pie'))
    print_barplot(data, args[2], append(args[3], 'Barplot'))
    print_boxplot(data, args[2], append(args[3], 'Boxplot'))
}
