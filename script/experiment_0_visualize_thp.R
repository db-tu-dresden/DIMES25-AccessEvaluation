source("script/general.R")
main_path <- "results/experiment_05paper/"
filetype <- "csv"
l_tex <- TRUE

color_value2 <- function(label){
    # https://davidmathlogic.com/colorblind/#%237CAE00-%23D05A52-%2300BFC4-%239F2FBB-%23374A52
    my_colors <- c("#7CAE00", "#D05A52", "#00BFC4", "#9F2FBB","#375251")
    names(my_colors) <- unique(label)
    return(my_colors)
}


paper_plot_special <- function(plot_data, filename, x_info, y_info, group_info, ld = FALSE, log_scale = FALSE, v_line_at = -1, max_val = 10000, dimensions = c(5.3, 2.2), rects = list(), tex = TRUE, multi_line = FALSE){
    if(tex){
        tikz(paste(filename,"tex", sep = "."), width = dimensions[1], height = dimensions[2])
    }
    if(ld){
        plot_data$x_axis <- log(plot_data$x_axis, 2)
    }
    # print(paste(group_info$color, names(group_info$color)))
    # print(unique(plot_data$print_name))
    # print(y_info$axis$breaks + 1)
    x_limits = c(fmin(plot_data$x_axis), fmax(plot_data$x_axis))
    p <- ggplot() +
        geom_line(data = plot_data[plot_data$print_name %!in% c("strided", "strided with hugepages"),], aes(x = x_axis, y = y_axis, group = print_name, color = print_name), linewidth = 1)+
        geom_line(data = plot_data[plot_data$print_name %in% c("strided with hugepages"),], aes(x = x_axis, y = y_axis, group = print_name, color = print_name), linewidth = 1)+
        geom_line(data = plot_data[plot_data$print_name %in% c("strided"),], aes(x = x_axis, y = y_axis, group = print_name, color = print_name), linewidth = 1)+
        theme_light() + 
        scale_y_continuous(
            name = y_info$label, 
            breaks = y_info$axis$breaks,
            labels = fix_break_labels(y_info$axis$breaks, ld = FALSE), 
            minor = y_info$axis$minor_breaks
        )+
        scale_color_manual(name = group_info$label, values = group_info$color)
        
    if(multi_line){
        p <- p + guides(color=guide_legend(ncol=2,byrow=TRUE))
    }
    p <- p + theme(legend.position = "top",
            panel.grid.major = element_line(linewidth = 0.6, color = "grey80"),
            panel.grid.minor = element_line(linetype = 'solid', linewidth = 0.2),
            plot.margin=unit(c(0, 0.5, 0.3, 0.3), units="lines"),
            legend.margin=margin(),
            legend.key.spacing.y = unit(-0.25, "lines"),
            # legend.box.margin=margin(-10,-10,-10,-10),
            # plot.background=element_rect(fill="red"),
            legend.box.spacing = unit(0, "lines")
    )
    for(r in rects){
        helper <- plot_data[plot_data$print_name %in% r$algs,]
        current_ymin <- fmin(helper[helper$x_axis >= r$xmin & helper$x_axis <= r$xmax,]$y_axis) * 0.9
        current_ymax <- fmax(helper[helper$x_axis >= r$xmin & helper$x_axis <= r$xmax,]$y_axis) * 1.03
        help_x <- r$xmin * r$xk + (1 - r$xk) * r$xmax
        help_y <- current_ymin * r$yk + current_ymax * (1 - r$yk)
        p <- p + annotate("rect", xmin = r$xmin, xmax = r$xmax, ymin = current_ymin, ymax = current_ymax, alpha = 0, color="grey25", linetype =2, linewidth = 0.4) +
            annotate("point", x=help_x, y = help_y, size = 3)+
            annotate("text", x = help_x, y = help_y, color ="white", label = r$text, size = 2.5)
    }
    if(v_line_at >= 0){
        p <- p + geom_vline(xintercept = v_line_at, linetype="dashed", color = "red", linewidth=0.75)
        # +
        # annotate("text", x = h_line_at, y = max(plot_data$ay), color = "grey25", label = "512 Partitions", hjust = - 0.05, size = 3)
    }
    if(!log_scale){
        p <- p + scale_x_continuous(
          name = x_info$label, 
          breaks = x_info$axis$breaks, 
          minor = x_info$axis$minor_breaks, 
          limits = x_limits, 
          expand = expansion(mult = c(0.01,0.01))
        )
    }else{
        final_labels <- fix_break_labels(x_info$axis$breaks, cutoff=max_val)
        p <- p + scale_x_continuous(
          name = x_info$label, 
          breaks = x_info$axis$breaks, 
          labels = final_labels,
          minor = x_info$axis$minor_breaks, 
          trans = "log2", 
          limits = x_limits, 
          expand = expansion(mult = c(0.01,0.01))
        )
    }
    if(tex){
        plot(p)
        dev.off()
    }
    ggsave(paste(filename, "pdf", sep = "."), width=dimensions[1], height=dimensions[2])
    ggsave(paste(filename, "png", sep = "."), width=dimensions[1], height=dimensions[2])
}


plot_stride <- function(agg_data, to_summarise, color_info, current_path = "", plot_wh = c(5.3, 2.2), x_axis_ticks = 0){
 
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
        paper_plot_special(plot_data, path,
            list(label = x_label, axis = x_axis),
            list(label = y_label, axis = y_axis),
            list(label = "", color = color_info), 
            log_scale = TRUE,
            dimensions = plot_wh, tex=l_tex
        )
    }
}

section2 <- function(agg_data, to_summarise, color_info){
    agg_data <- agg_data[agg_data$algorithm != "strided unrolled",]
    path <- "combination/"
    make_dir(path)
    plot_stride(agg_data, to_summarise, current_path = path, color_info = color_info, plot_wh = c(6, 2.2))
}

setwd(main_path)

raw_filename <- "1_gib_local_DRAM_this"
raw_thp_filename <- "thp_1_gib_local_DRAM_3"    

raw_data <- fread(paste(raw_filename, filetype, sep = "."), data.table=FALSE)
raw_thp_data <- fread(paste(raw_thp_filename, filetype, sep = "."), data.table=FALSE)

print(raw_data[1:10,])
print(raw_thp_data[1:10,])


raw_data <- raw_data[,which(colnames(raw_data) %in% colnames(raw_thp_data))]
raw_thp_data <- raw_thp_data[raw_thp_data$algorithm == "strided",which(colnames(raw_thp_data) %in% colnames(raw_data))]
raw_thp_data$algorithm <- "strided with hugepages"

print(raw_data[1:10,])
print(raw_thp_data[1:10,])

raw_data <- rbind(raw_data, raw_thp_data)
#todo

cat(paste(" ", get_time_string(fsum(raw_data$time_ns)), "\t"))
to_summarise <- get_aggregation_labels(raw_data)

for(metric in to_summarise){
    if(metric == "cycles"){
        colnames(raw_data)[which(colnames(raw_data) == "cycles")] <- "Cycles"
    }
    # todo .. this needs to be done for every counter that has an unsightly name
    # IMPORTANT don't change the name of gibs. it is a reference point AND gets changed later on anyways.
    # FOR changeing the name in plots see get_y_label in general.R
    # for an even better label this can also be used.
    # please be aware that r is a bitch and doesn't let me escape _ for latex so at one point i just replace it with -
}

to_summarise <- get_aggregation_labels(raw_data)

agg_data <- raw_data %>% fgroup_by(algorithm, data_amount, byte_count, stride) %>% fmedian()

color_info <- color_value2(agg_data$algorithm)

section2(agg_data, to_summarise, color_info)
# write_success(raw_filename)

