source("script/general.R")
main_path <- "results/experiment_fixed_data_size/"
filetype <- "csv"
l_tex <- FALSE

rects <- list(
    list(xmin = 1000, xmax = 4096, xk = 0.1, yk = 0.15, text = "1", algs = c("strided")), 
    list(xmin = 1, xmax = 128, xk = 0.1, yk = 0.05, text = "2", algs = c("strided", "strided unrolled")), 
    list(xmin = 14, xmax = 18, xk = 0.925, yk = -0.09, text = "3", algs = c("strided", "strided unrolled"))
)

plot_stride <- function(agg_data, to_summarise, file_data, color_info, current_path = "", plot_wh = c(5.3, 2.2), x_axis_ticks = 0){
 
    if(x_axis_ticks == 0){
        x_axis_ticks = number_x_axis_ticks
    }
    colnames(agg_data)[which(colnames(agg_data) == "stride")] <- "x_axis"
    pre_plot_data_plus_info <- better_representation_of_few_data_points(agg_data)
    pre_plot_data <- pre_plot_data_plus_info$frame

    simple_line_labels <- pre_plot_data_plus_info$info
    x_label <- "stride size"
    x_axis <- get_paper_breaks(pre_plot_data$x_axis, x_axis_ticks, power_two = TRUE, ld_ticks = TRUE)

    for(metric in to_summarise){
        plot_data <- pre_plot_data[,which(colnames(pre_plot_data) %in%c("print_name", "x_axis", metric))]
        colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
        y_label <- get_y_label(metric)
        y_axis <- get_breaks(plot_data$y_axis, number_y_axis_ticks, start_at_zero = FALSE)

        path <- paste(current_path, metric, sep = "/")
        paper_plot(plot_data, path,
            list(label = x_label, axis = x_axis),
            list(label = y_label, axis = y_axis),
            list(label = "", color = color_info), 
            log_scale = TRUE, dimensions = plot_wh, tex=l_tex
        )
    }
}

plot_partitions <- function(agg_data, to_summarise, file_data, color_info, current_path = "", plot_wh = c(5.3, 2.2), x_axis_ticks = 0, rects = list()){
    agg_data$x_axis <- agg_data$data_amount * 1.0 / agg_data$stride

    if(x_axis_ticks == 0){
        x_axis_ticks = number_x_axis_ticks
    }
        
    c_data <- agg_data
    pre_plot_data_plus_info <- better_representation_of_few_data_points(c_data)
    pre_plot_data <- pre_plot_data_plus_info$frame
    simple_line_labels <- pre_plot_data_plus_info$info
    x_label <- "Partition Count"
    x_axis <- get_paper_breaks(pre_plot_data$x_axis, x_axis_ticks, power_two = TRUE, start_at_zero = TRUE, ld_ticks = TRUE)
    # stop()
    for(metric in to_summarise){
        plot_data <- pre_plot_data[,which(colnames(pre_plot_data) %in%c("print_name", "x_axis", metric))]
        colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
        y_label <- gsub("_", "-", metric)
        if(metric == "gibs"){y_label <- "GiB / s"}
        y_axis <- get_breaks(plot_data$y_axis, number_y_axis_ticks, start_at_zero = FALSE)
        
        path <- paste(current_path, metric, sep = "/")
        paper_plot(plot_data, path,
            list(label = x_label, axis = x_axis),
            list(label = y_label, axis = y_axis),
            list(label = "", color = color_info), 
            log_scale = TRUE,
            dimensions = plot_wh, tex=l_tex
        )
    }
}

plot_partitions <- function(
    agg_data, to_summarise, 
    file_data, color_info, 
    current_path = "", plot_wh = c(5.3, 2.2), 
    x_axis_ticks = 0, rects = list(), 
    log_scale = TRUE, start_at_zero = TRUE, 
    ld_ticks = TRUE, ld_steps = TRUE, get_info = FALSE,
    limits = c(-Inf, Inf), use_only_algs = c()
){
    
    agg_data$x_axis <- agg_data$real_data_amount * 1.0 / agg_data$stride
    agg_data <- agg_data[agg_data$x_axis >= limits[1] & agg_data$x_axis < limits[2] + 1,]
    if(nrow(agg_data)>1){
        if(x_axis_ticks == 0){
            x_axis_ticks = number_x_axis_ticks
        }
            
        c_data <- agg_data
        pre_plot_data_plus_info <- better_representation_of_few_data_points(c_data)
        pre_plot_data <- pre_plot_data_plus_info$frame
        simple_line_labels <- pre_plot_data_plus_info$info
        x_label <- "Partition Count"
        x_axis <- get_paper_breaks(c(fmin(pre_plot_data$x_axis),fmax(pre_plot_data$x_axis)), x_axis_ticks, power_two = log_scale, start_at_zero = start_at_zero, ld_ticks = ld_ticks, ld_steps = ld_steps)

        if(length(use_only_algs) > 0){
            pre_plot_data <- pre_plot_data[pre_plot_data$print_name %in% use_only_algs,]
        }
        if(nrow(pre_plot_data) > 1){
            if(get_info){
                get_general_info(agg_data, to_summarise, current_path, x_label)
            }

            for(metric in to_summarise){
                plot_data <- pre_plot_data[,which(colnames(pre_plot_data) %in%c("print_name", "x_axis", metric))]
                colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
                
                y_label <- get_y_label(metric)
                y_axis <- get_breaks(plot_data$y_axis, number_y_axis_ticks, start_at_zero = FALSE)
                
                path <- paste(current_path, metric, sep = "/")
                paper_plot(plot_data, path,
                    list(label = x_label, axis = x_axis),
                    list(label = y_label, axis = y_axis),
                    list(label = "", color = color_info), 
                    log_scale = log_scale,
                    dimensions = plot_wh, rects = rects, tex=l_tex)
            }
        }
    }
}


plot_partitions_2_y <- function(
    agg_data, to_summarise, 
    file_data, color_info, 
    current_path = "", plot_wh = c(5.3, 2.2), 
    x_axis_ticks = 0, rects = list(), 
    log_scale = TRUE, start_at_zero = TRUE, 
    limits = c(-Inf, Inf), use_only_algs = c()
){
    agg_data$x_axis <- agg_data$data_amount * 1.0 / agg_data$stride
    agg_data <- agg_data[agg_data$x_axis >= limits[1] & agg_data$x_axis <= limits[2],]
    if(nrow(agg_data)>1){
        if(x_axis_ticks == 0){
            x_axis_ticks = number_x_axis_ticks
        }
        
        c_data <- agg_data
        pre_plot_data_plus_info <- better_representation_of_few_data_points(c_data)
        pre_plot_data <- pre_plot_data_plus_info$frame
        simple_line_labels <- pre_plot_data_plus_info$info
        x_label <- "Partition Count"
        x_axis <- get_paper_breaks(pre_plot_data$x_axis, x_axis_ticks, power_two = log_scale, start_at_zero = start_at_zero, ld_ticks = log_scale)

        if(length(use_only_algs) > 0){
            pre_plot_data <- pre_plot_data[pre_plot_data$algorithm %in% use_only_algs,]
        }
        if(nrow(pre_plot_data) > 1){
            to_summarise <- to_summarise[to_summarise != "gibs"]
            
            colnames(pre_plot_data)[which(colnames(pre_plot_data) == "gibs")] <- "y1_axis"

            for(metric in to_summarise){
                plot_data <- pre_plot_data[,which(colnames(pre_plot_data) %in%c("print_name", "x_axis", "y1_axis", metric))]
                colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y2_axis"

                y1_label <- get_y_label("gibs")
                y2_label <- get_y_label(metric)


                all_y_axis <- get_paper_multi_axis_breaks(plot_data$y1_axis, plot_data$y2_axis, number_y_axis_ticks, start_at_zero = FALSE)

                if(length(unique(plot_data$y2_axis)) > 1){
                    
                    path <- paste(current_path, metric, sep = "/")
                    paper_plot_2_y(plot_data, path,
                        list(label = x_label, axis = x_axis),
                        list(label1 = y1_label, label2 = y2_label, axis = all_y_axis, metric1 = "gibs", metric2 = metric),
                        list(label = "", color = color_info), 
                        log_scale = log_scale,
                        dimensions = plot_wh, rects = rects, tex=l_tex)
                
                }
            }
        }
    }
}

section2 <- function(agg_data, to_summarise, file_data, color_info){
    agg_data <- agg_data[agg_data$algorithm != "strided unrolled",]
    path <- paste(file_data$filename, "section2", sep = "/")
    make_dir(path)
    plot_stride(agg_data, to_summarise, file_data, current_path = path, color_info = color_info)
}

section3_sub <- function(agg_data, to_summarise, file_data, current_path, color_info, rect){
    path <- paste(current_path, rect$text, sep = "/")
    make_dir(path)
    
    plot_partitions(agg_data, to_summarise, file_data, current_path = path, plot_wh = c(2.6, 2.2), x_axis_ticks = 6, color_info = color_info, limits = c(rect$xmin, rect$xmax), use_only_algs = rect$algs, log_scale = FALSE, start_at_zero = TRUE, ld_ticks = FALSE, ld_steps = TRUE)
    
    path <- paste(current_path, rect$text, "2_y", sep = "/")
    make_dir(path)
    
    plot_partitions_2_y(agg_data, to_summarise, file_data, current_path = path, plot_wh = c(5.3, 2.2), x_axis_ticks = 6, color_info = color_info, limits = c(rect$xmin, rect$xmax), use_only_algs = rect$algs, log_scale = FALSE, start_at_zero = FALSE)
}

section3_0 <- function(agg_data, to_summarise, file_data, current_path, color_info){
    path <- paste(current_path, "0", sep = "/")
    make_dir(path)
    plot_partitions(agg_data, to_summarise, file_data, current_path = path, c(10, 2.2), x_axis_ticks = 16, color_info = color_info, rects = rects, get_info = FALSE)
    for(r in rects){
        section3_sub(agg_data, to_summarise, file_data, current_path, color_info, r)
    }
}

section3 <- function(agg_data, to_summarise, file_data, color_info){
    path <- paste(file_data$filename, "section3", sep = "/")
    make_dir(path)
    section3_0(agg_data, to_summarise, file_data, current_path = path, color_info = color_info)
}

all_plot_file_data <- get_all_new_files(main_path, filetype)

all_plot_file_data <- get_all_new_files(main_path, filetype, ignore_already_plotted=TRUE)
if(length(all_plot_file_data) == 0){
    cat(paste("No (new) csv file found in ", main_path, "  Skipping...", "\n", sep=""))
}else{
    cat(paste("experiment_fixed_data_size new files found in ", main_path, " plotting:\t", sep = ""))
    setwd(main_path)
    for(file_data in all_plot_file_data){
        cat(paste(file_data$filename, filetype, sep = "."))
        
        raw_data <- fread(paste(file_data$filename, filetype, sep = "."), data.table=FALSE)
        cat(paste(" took ", get_time_string(fsum(raw_data$time_ns)), ", \t", sep = ""))
        to_summarise <- get_aggregation_labels(raw_data)

        for(metric in to_summarise){
            if(metric == "cycles"){
                colnames(raw_data)[which(colnames(raw_data) == "cycles")] <- "Cycles"
            }
        }

        to_summarise <- get_aggregation_labels(raw_data)

        agg_data <- raw_data %>% fgroup_by(algorithm, data_amount, byte_count, stride) %>% fmedian()

        color_info <- color_value(agg_data$algorithm)

        section2(agg_data, to_summarise, file_data, color_info)
        section3(agg_data, to_summarise, file_data, color_info)
        write_success(file_data)
    }

    cat("\n\n")
}
