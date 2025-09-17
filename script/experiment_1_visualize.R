source("script/general.R")
main_path <- "results/experiment_fixed_stride_size/"
filetype <- "csv"

plot_partitions <- function(agg_data, to_summarise, file_data, current_path, plot_wh = c(5.3, 2.2), x_axis_ticks = 0){    
    if(x_axis_ticks == 0){
        x_axis_ticks = number_x_axis_ticks
    }

    colnames(agg_data)[colnames(agg_data) == "lane_count"] <- "x_axis"
    color_label <- color_value(unique(agg_data$algorithm))

    
    pre_plot_data_plus_info <- better_representation_of_few_data_points(agg_data)
    pre_plot_data <- pre_plot_data_plus_info$frame
    simple_line_labels <- pre_plot_data_plus_info$info
    x_label <- "Partition Count"
    x_axis <- get_paper_breaks(pre_plot_data$x_axis, number_x_axis_ticks, power_two = TRUE, start_at_zero = TRUE)
    for(metric in to_summarise){
        plot_data <- pre_plot_data[,which(colnames(pre_plot_data) %in%c("print_name", "x_axis", metric))]
        colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
        
        y_label <- metric
        if(metric == "gibs"){y_label <- "GiB / s"}
        
        y_axis <- get_breaks(plot_data$y_axis, number_y_axis_ticks, start_at_zero = FALSE)
        lt_values <- linetype_value(plot_data$print_name, simple_line_labels)

        path <- paste(current_path, metric, sep = "/")
        paper_plot(plot_data, path,
            list(label = x_label, axis = x_axis),
            list(label = y_label, axis = y_axis),
            list(label = "Stride Size", values = lt_values, color = color_label),
            dimensions = plot_wh, tex=l_tex, align_legend_left = TRUE
        )
    }
}

files <- c("127_MiB", "127b_MiB", "128_MiB")
setwd(main_path)

raw_data <- fread(paste(files[[1]], filetype, sep = "."), data.table=FALSE)
for(i in seq(2, length(files), 1)){
    next_raw <- fread(paste(files[[i]], filetype, sep = "."), data.table=FALSE)

    raw_data <- raw_data[, which(colnames(raw_data) %in% colnames(next_raw))]
    next_raw <- next_raw[, which(colnames(next_raw) %in% colnames(raw_data))]

    raw_data <- rbind(raw_data, next_raw)
}

to_summarise <- get_aggregation_labels(raw_data)

agg_data <- raw_data %>% fgroup_by(algorithm, data_amount, real_data_amount, byte_count, stride, lane_count) %>% fmedian()

all_algs <- unique(agg_data$algorithm)

for(alg in all_algs){
    
    path <- paste("combination", alg, "", sep = "/")
    make_dir(path)
    work_data <- agg_data[agg_data$algorithm == alg,] 
    work_data$algorithm <- paste(round(work_data$stride / 1024 / 128, 2), "MiB")
    mibs <- unique(work_data$algorithm)
    best <- work_data[0,]
    for(m in mibs){
        fits <- work_data[work_data$algorithm == m,]
        fits <- fits[order(fits$gibs, decreasing = TRUE),]
        fits <- fits[1:3,]
        best <- rbind(best, fits)
    }
    cat("Top results per stride size\n")
    print(best)
    cat("\n")
    
    color_info <- color_value(work_data$algorithm)
    plot_partitions(work_data, to_summarise, color_info, path, plot_wh = c(3.6,1.9))
}
