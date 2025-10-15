source("script/general.R")
main_path <- "results/experiment_fixed_partition_count/"
filetype <- "csv"
l_tex <- FALSE

plot_stride <- function(agg_data, to_summarise, color_info, current_path = "", plot_wh = c(5.3, 2.2), x_axis_ticks = 0){
 
    if(x_axis_ticks == 0){
        x_axis_ticks = number_x_axis_ticks
    }
    agg_data$mb <- agg_data$stride * 8. / 1024 / 1024

    colnames(agg_data)[which(colnames(agg_data) == "mb")] <- "x_axis"
    pre_plot_data_plus_info <- better_representation_of_few_data_points(agg_data)
    pre_plot_data <- pre_plot_data_plus_info$frame

    simple_line_labels <- pre_plot_data_plus_info$info
    x_label <- "Stride Size (MiB)"
    x_axis <- get_paper_breaks(pre_plot_data$x_axis, ceiling(x_axis_ticks/2)+1, power_two = TRUE, ld_ticks = FALSE)

    for(metric in to_summarise){
        plot_data <- pre_plot_data[,which(colnames(pre_plot_data) %in%c("print_name", "x_axis", metric))]
        colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
        y_label <- get_y_label(metric)
        y_axis <- get_breaks(c(2,plot_data$y_axis), number_y_axis_ticks, start_at_zero = FALSE)
        
        path <- paste(current_path, metric, sep = "/")
        paper_plot(plot_data, path,
            list(label = x_label, axis = x_axis),
            list(label = y_label, axis = y_axis),
            list(label = "No. of Partitions", color = color_info), 
            dimensions = plot_wh, tex=l_tex
        )
    }
}


files <- c("13P", "33P", "42P")

setwd(main_path)

raw_data <- fread(paste(files[1], filetype, sep = "."), data.table=FALSE)
for(i in seq(2, length(files), 1)){
    next_raw <- fread(paste(files[i], filetype, sep = "."), data.table=FALSE)

    raw_data <- raw_data[, which(colnames(raw_data) %in% colnames(next_raw))]
    next_raw <- next_raw[, which(colnames(next_raw) %in% colnames(raw_data))]

    raw_data <- rbind(raw_data, next_raw)
}   
cat(paste("experiment_fixed_partition_count took: ", get_time_string(fsum(raw_data$time_ns)), "\n"))
to_summarise <- get_aggregation_labels(raw_data)

agg_data <- raw_data %>% fgroup_by(algorithm, data_amount, real_data_amount, byte_count, stride, lane_count) %>% fmedian()

all_algs <- unique(agg_data$algorithm)

for(alg in all_algs){
    make_dir(alg)
    work_data <- agg_data[agg_data$algorithm == alg,] 
    work_data$algorithm <- as.factor(work_data$lane_count)

    color_info <- color_value(work_data$algorithm)
    plot_stride(work_data, to_summarise, color_info, paste(alg, "", sep = "/"), plot_wh = c(3.6, 1.9))
}
cat("\n")
