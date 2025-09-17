library(magrittr)
library(dplyr, warn.conflicts = FALSE)
library(ggplot2)
library(data.table, warn.conflicts = FALSE)
library(tikzDevice)
library(latex2exp)
suppressMessages(library(collapse, warn.conflicts = FALSE))

options("width"=200)

l_tex <- FALSE

plot_width <- 5.3
plot_heigth <- 2.2
number_x_axis_ticks <- 8
number_y_axis_ticks <- 5

is.integer0 <- function(x){
  is.integer(x) && length(x) == 0L
}

is.prime <- function(num) {
    if (num == 2) {
        return(TRUE)
    } else if (any(num %% 2:(num-1) == 0)) {
        return(FALSE)
    } else { 
        return(TRUE)
    }
}

is.prime_skewed <- function(num, div) {
    if (any(num %% div == 0)) {
        return(FALSE)
    } else { 
        return(TRUE)
    }
}


'%!in%' <- function(x,y)!('%in%'(x,y))

sort_head <- function(df, var.name, n){
    df1 <- df[rev(order(df[[var.name]])), ]
    sorted <- head(df1, n)
    return(sorted)
}


only_common_labels <- function(all_raw_data){
    for(i1 in 1:length(all_raw_data)){
        for(i2 in 1:length(all_raw_data)){
            all_raw_data[[i1]] <- all_raw_data[[i1]][,colnames(all_raw_data[[i1]]) %in% colnames(all_raw_data[[i2]])]
        }
    }
    result <- all_raw_data[[1]]
    for(i in 2:length(all_raw_data)){
        result <- rbind(result, all_raw_data[[i]])
    }
    
    return(result)
}


get_general_info <- function(data, aggregants, name, label){
    number <- 5
    wowie <- aggregants[aggregants != "gibs"]
    data <- data[,names(data) %!in% wowie]
    
    cat(paste("\n",name, "\n",sep = "\t"))
    algs <- unique(data$algorithm)
    colnames(data)[which(colnames(data) == "x_axis")] <- label
    cat(paste(paste(names(data),collapse = ", "), "\n"))
    # for(metric in aggregants){
    metric <- "gibs"
    for(alg in algs){
        sub_a <- data[data$algorithm == alg,]
        sub_f <- sort_head(sub_a, metric, number)
        for(i in seq(1, number)){
            cat(paste(paste(sub_f[i,],collapse = ", "), "\n"))
        }
        cat("\n")
    }
    # }
}

#function to write a timestamp file to the result folder. This should help us to avoid replotting every csv if we use 'make plots'
write_success <- function(current){
    write(current$timestamp, paste(current$filename, "/timestamp.txt", sep = ""))
}

get_time_string <- function(time_ns){
    time <- time_ns / 1000000000.
    time_string <- ""
    if(time > 1 * 60 * 60 * 24){
        days <- floor(time / (60 * 60 * 24))
        time_string <- paste(days, "d", sep = "")
        time = time - days * 60 * 60 * 24 
    }
    h <- floor(time / (60 * 60))
    time = time - h * 60 * 60
    m <- floor(time / 60)
    time = time - m * 60
    s <- floor(time)
    time_string <- paste(time_string, paste(h,m,s, sep=":"))
    return (time_string)   
}

get_all_new_files <- function(path, filetype, ignore_already_plotted = FALSE, ignore = FALSE, clean = FALSE, plot_filenames = c("")){
    filenames <- list.files(path, pattern=paste("*.", filetype,sep=""), full.names = FALSE)
    make_plot <- filenames != ""

    file_names_only <- remove_file_type(filenames)
    
    result <- data.frame(file=filenames, filename=remove_file_type(filenames))
    result$timestamp <- ""
    result$use <- 1
    # print(result)

    for(i in 1:nrow(result)){
        result[i,]$timestamp <- paste(file.info(paste(path,result[i,]$file, sep =""))$ctime, "", sep = "")
        
        dir_path <- paste(path, result[i,]$filename, "/",sep="");
        time_path <- paste(dir_path, "timestamp.txt", sep="");
        if(dir.exists(dir_path)){
            if(file.exists(time_path)){
                prev_timestamp = readLines(time_path)[1]
                if(prev_timestamp == result[i,]$timestamp && !(ignore_already_plotted || ignore)){
                    # print(paste(prev_timestamp, "==", result[i,]$timestamp))
                    result[i,]$use = 0
                }else{
                    # print(paste(prev_timestamp, "<>", result[i,]$timestamp))
                    make_dir(dir_path, clean)
                }
            }
        }else{
            make_dir(dir_path)
        }
    }
    # print(paste("filenames: ", paste(plot_filenames, collapse = ", "), "    new filenames: ", paste(result$file, collapse = ", "), sep = " "))
    if(nrow(result[result$file %in% plot_filenames,]) > 0){
        result[result$file %in% plot_filenames,]$use <- 1
    }
    return (apply(result[result$use > 0,], 1, as.list))
}

remove_file_type <- function(filenames){
    intermediate <- unlist(strsplit(filenames, "\\."))
    names <- intermediate[seq(1, length(intermediate), 2)]
    return(names)
}

make_dir <- function(path, clean = FALSE){
    exists <- dir.exists(path)
    if(exists && clean){
        unlink(path, recursive = TRUE)
        dir.create(path, recursive = TRUE)
    }else if(!exists){
        dir.create(path, recursive = TRUE)
    }
}

#todo if you want pretty labels. if not ignore
get_y_label <- function(metric){
    if(metric == "gibs"){return("GiB / s")}
    if(metric == "CYCLE_ACTIVITY.STALLS_TOTAL"){return("Total Stalls")}

    # I need to replace _ with - because R is a bitch and latex doesn't like unescaped _    
    return(gsub("_", "-", metric))
    # return("Counter Values")
}

get_breaks <- function(values, ticks, power_two = FALSE, start_at_zero = TRUE, ld_ticks = FALSE){
  if(ld_ticks){
    values <- log(values, 2)
  }
  
  power <- 10
  exp_sub <- 1
  if(power_two){
    power <- 2
    exp_sub <- log(ticks, power) 
  }
  
  ticks <- ticks - 1
  
  min_val <- floor(fmin(values))
  
  max_val <- ceiling(fmax(values))
  
  space <- max_val - min_val
  exp <- 0
  if(space >= 4000){
    exp <- floor(log(space, power) - exp_sub)
  }
  
  space <- ceiling(space * '^'(power, -exp))
  e_min_val <- floor(min_val * '^'(power, -exp))
  e_max_val <- floor(max_val * '^'(power, -exp))
  space <- e_max_val - e_min_val
  
  if(power_two){
    break_stepsize <- '^'(2, round(log(space / ticks, 2)))
  }else{
    break_stepsize <- round(space / ticks)
  }
  
  while(break_stepsize > 5 && is.prime_skewed(break_stepsize, c(2, 3, 5))){
    break_stepsize <- break_stepsize - 1
  }
  if(break_stepsize == 0){
    break_stepsize <- round(space * 10.0 / ticks)/10
  }
  minor_breaks_stepsize <- break_stepsize / 2;
  if(break_stepsize %% 3 == 0){
    minor_breaks_stepsize <- break_stepsize / 3;
  }else if(break_stepsize %% 4 == 0){
    minor_breaks_stepsize <- break_stepsize / 4;
#   }else if(break_stepsize %% 5 == 0){
#     minor_breaks_stepsize <- break_stepsize / 5;
  }
  breaks <- c(0)
  minor_breaks <- c(0)
  if(start_at_zero){
    breaks <- unique(c(seq(0, e_max_val + break_stepsize, break_stepsize), e_min_val))
    minor_breaks <- unique(c(seq(0, e_max_val + minor_breaks_stepsize, minor_breaks_stepsize), e_min_val))
  }else{
    breaks <- unique(seq(e_min_val, e_max_val + break_stepsize, break_stepsize))
    minor_breaks <- unique(seq(e_min_val, e_max_val + minor_breaks_stepsize, minor_breaks_stepsize))
  }
  
  breaks <- breaks * '^'(power, exp)
  minor_breaks <- minor_breaks * '^'(power, exp)
  # print(paste(minor_breaks_stepsize, "wowie",paste( minor_breaks, collapse = ", " )))
  # breaks <- breaks[breaks >= min_val]
  # minor_breaks <- minor_breaks[minor_breaks >= min_val]
  
  if(ld_ticks){
    breaks <- '^'(2, breaks)
    minor_breaks <- '^'(2, minor_breaks)
  }
  return(list(breaks= breaks, minor_breaks = minor_breaks))
}

fix_break_labels <- function(input, ld = TRUE, cutoff = 0){
    
    labels <- input
    if(length(input) == 1 & input[1] == 0){
        return(c(0))
    }
    if(ld){
        if(cutoff == 0)
            cutoff <- 4096
        str <- paste(labels)    
        str2 <- paste("$2^{",log(labels, 2), "}$", sep = "")
        final_labels <- str
        final_labels[labels > cutoff] <- str2[labels > cutoff]
        final_labels <- TeX(final_labels)
        return (final_labels)
    }

    if(cutoff == 0)
        cutoff <- 1000
        
    exp <- floor(log(input, 10))
    exp <- exp[exp > -1000 & exp < 1000]
    if(length(exp) == 0){
        exp <- 0
    }else{
        exp <- min(exp)
    }
    factor <- input / '^'(10, exp)

    str <- paste(input)
    str2 <- paste("$", factor ,"\\cdot 10^{", exp,"}$", sep = "")
    str3 <- paste("$10^{", exp,"}$", sep = "")
    final_labels <- str
    final_labels[labels > cutoff] <- str2[labels > cutoff]
    final_labels[labels > cutoff & factor == 1] <- str3

    final_labels <- TeX(final_labels)
    return (final_labels)
}

get_paper_breaks <- function(values, ticks, power_two = FALSE, start_at_zero = TRUE, ld_ticks = FALSE, ld_steps = FALSE, probs_add_min = TRUE){

    if(ld_ticks){
        values <- log(values, 2)
    }

    power <- 10
    exp_sub <- 1
    if(power_two){
        power <- 2
        exp_sub <- log(ticks, power) 
    }

    if(exp_sub >= 2){
        exp_sub <- exp_sub - 2
    }else if(exp_sub >= 1){
        exp_sub <- exp_sub - 1
    }

    ticks <- ticks - 1

    min_val <- floor(fmin(values))

    max_val <- ceiling(fmax(values))

    space <- max_val - min_val
    exp <- 0
    if(space >= 4000){
        exp <- floor(log(space, power) - exp_sub)
    }

    space <- ceiling(space * '^'(power, -exp))
    e_min_val <- floor(min_val * '^'(power, -exp))
    e_max_val <- floor(max_val * '^'(power, -exp))
    space <- e_max_val - e_min_val

    break_stepsize <- round(space / ticks)
    if(ld_steps && break_stepsize > 1){
        break_stepsize <- '^'(2,round(log(break_stepsize,2)))
    }
    # print(break_stepsize)
    while(break_stepsize > 5 && is.prime_skewed(break_stepsize, c(2, 3, 5))){
        break_stepsize <- break_stepsize - 1
    }
    if(break_stepsize == 0){
        break_stepsize <- round(space * 10.0 / ticks)/10
    }
    minor_breaks_stepsize <- break_stepsize / 4; #todo change back to 2
    if(break_stepsize %% 3 == 0){
        minor_breaks_stepsize <- break_stepsize / 3;
    }else if(break_stepsize %% 4 == 0){
        minor_breaks_stepsize <- break_stepsize / 4;
    }else if(break_stepsize %% 5 == 0){
        minor_breaks_stepsize <- break_stepsize / 5;
    }

    breaks <- c(0)
    minor_breaks <- c(0)
    if(start_at_zero){
        breaks <- seq(0, e_max_val + break_stepsize, break_stepsize)
        minor_breaks <- unique(c(seq(0, e_max_val + minor_breaks_stepsize, minor_breaks_stepsize), seq(0, 0 -minor_breaks_stepsize, -minor_breaks_stepsize)))
        if(min_val < 10 && probs_add_min){
            breaks <- unique(c(breaks, e_min_val))
        }
    }else{
        breaks <- unique(seq(e_min_val, e_max_val + break_stepsize, break_stepsize))
        minor_breaks <- unique(c(seq(e_min_val, e_max_val + minor_breaks_stepsize, minor_breaks_stepsize), seq(e_min_val, e_min_val - minor_breaks_stepsize, -minor_breaks_stepsize)))
    }

    breaks <- breaks * '^'(power, exp)
    minor_breaks <- minor_breaks * '^'(power, exp)

    # breaks <- breaks[breaks >= min_val]
    # minor_breaks <- minor_breaks[minor_breaks >= min_val]
    breaks <- breaks[breaks <= max_val]
    breaks <- breaks[breaks >= min_val]

    if(ld_ticks){
        breaks <- '^'(2, breaks)
        minor_breaks <- '^'(2, minor_breaks)
    }
    # print(breaks)
    return(list(breaks= breaks, minor_breaks = minor_breaks))
}

get_paper_multi_axis_breaks <- function(values1, values2, ticks, power_two = FALSE, start_at_zero = TRUE, ld_ticks = FALSE, ld_steps = FALSE, probs_add_min = TRUE){
    
    vrange1 <- c(fmin(values1), fmax(values1))
    vrange2 <- c(fmin(values2), fmax(values2))

    if(all(vrange2 == c(0,0))){
        mult <- 1
    }else{
        mult <- vrange1/vrange2
        mult <- mult[2]
        exp <- floor(log(mult, 10))
        power <- mult * '^'(10, -exp)

        #do these steps to get a nicer secondary y-axis
        factor <- c(2, 4, 5, 10)
        diff <- abs(power - factor) 
        dir <- -1 * (power > factor) + 1 * (power < factor) 
        smallest_diff <-c(which(diff == min(diff)), 0)
        power <- power + dir[smallest_diff[1]] * diff[smallest_diff[1]]

        mult <- power * '^'(10, exp)    
    }
    
    values <- c(vrange1, mult * vrange2)
    # print(paste("values: ", paste(values, collapse = ",")))
    res <- get_paper_breaks(values, ticks = ticks, power_two = power_two, start_at_zero = start_at_zero, ld_ticks = ld_ticks, ld_steps = ld_steps, probs_add_min = probs_add_min)
    res$coeff <- mult
    res$breaks_y2 <- res$breaks / res$coeff
    
    return(res)
}

get_aggregation_labels <- function(frame, start_aggregation_at = "gibs"){
    if(is.integer0(which( colnames(frame)==start_aggregation_at ))){
        stop(paste("Couldn't find starting column <", start_aggregation_at, "> Need this column to list all columns to aggregate"))
    }
    to_summarise <- colnames(frame)[seq(which( colnames(frame)==start_aggregation_at ), ncol(frame))]   
    return(to_summarise)
}

better_representation_of_few_data_points <- function(frame, column_for_name_plus = ""){
    if(is.integer0(which(colnames(agg_data) == "byte_count"))){
        alg_information <- frame %>% fgroup_by(algorithm, x_axis) %>% fcount(algorithm)
    }else{
        alg_information <- frame %>% fgroup_by(algorithm, byte_count) %>% fcount(algorithm)
    }
    algorithm_info <- as.data.frame(alg_information)

    complex_line <- algorithm_info[algorithm_info$N > 1,]$algorithm
    simple_data <- frame[not(frame$algorithm %in% complex_line),]
    complex_data <- frame[frame$algorithm %in% complex_line,]
    complex_data$print_name <- complex_data$algorithm

    
    if(column_for_name_plus != ""){
        complex_data$print_name <- paste(complex_data$algorithm, complex_data[, which(colnames(complex_data) == column_for_name_plus)], sep = " ")
    }
    if(nrow(simple_data) > 0){
        simple_data$print_name <- simple_data$algorithm

        simple_data_help <- simple_data
        simple_data$x_axis<- fmin(complex_data$x_axis)
        simple_data_help$x_axis <- fmax(complex_data$x_axis)

        pre_plot_data <- rbind(simple_data, simple_data_help, complex_data)
        return(list(frame = pre_plot_data, info = unique(simple_data$print_name)))
    }
    return(list(frame = complex_data, info = c("ERROR / No simple line")))
}

linetype_value <- function(label, different_type){
    lt_names <- label
    lt_values <- lt_names %in% different_type
    lt_values <- lt_values * 1 + 1
    names(lt_values) <- lt_names
    return(lt_values)
}

color_value <- function(label){
    # https://davidmathlogic.com/colorblind/#%237CAE00-%23D05A52-%2300BFC4-%239F2FBB
    my_colors <- c("#7CAE00", "#D05A52", "#00BFC4", "#9F2FBB")
    names(my_colors) <- unique(label)
    return(my_colors)
}

general_plot <- function(plot_data, filename, x_info, y_info, group_info, ld = FALSE, log_scale = FALSE, v_line_at = -1, y_limits = c(-1, -1), band = FALSE, dimensions = c(5.3, 2.2)){
  if(ld){
    plot_data$x_axis <- log(plot_data$x_axis, 2)
  }
  if(y_limits[1] < 0 | y_limits[2] < 0){
    y_limits = c(fmin(plot_data$y_axis), fmax(plot_data$y_axis))
  }
  p <- ggplot(data = plot_data, aes(x = x_axis, y = y_axis, group = print_name)) +
    geom_line(linewidth = 1, aes(color = print_name, linetype = print_name))
    
    if(band){
        p<- p + geom_ribbon(aes(ymin=y_axis_min, ymax = y_axis_max, fill = print_name), alpha=0.2) +
        scale_fill_manual(name = group_info$label, values = group_info$color)
    }
    p <- p + theme_light() + 
    scale_y_continuous(name = y_info$label, breaks = y_info$axis$breaks, minor = y_info$axis$minor_breaks, limits = y_limits)+
    scale_color_manual(name = group_info$label, values = group_info$color)+
    scale_linetype_manual(name = group_info$label, values = group_info$values)+
    # theme(legend.position.inside = c(0, 7, 0, 7),
    #       panel.grid.major = element_line(linewidth = 0.6, color = "grey80"),
    #       panel.grid.minor = element_line(linetype = 'dashed', linewidth = 0.2))
    theme(legend.position = "top",
            panel.grid.major = element_line(linewidth = 0.6, color = "grey80"),
            panel.grid.minor = element_line(linetype = 'solid', linewidth = 0.2),
            legend.margin=margin(1,1,1,1),
            legend.box.margin=margin(-5,-10,-10,-10))

  if(v_line_at >= 0){
    p <- p + geom_vline(xintercept = v_line_at, linetype="dashed", color = "red", linewidth=0.75)
    # +
    # annotate("text", x = h_line_at, y = max(plot_data$ay), color = "grey25", label = "512 Partitions", hjust = - 0.05, size = 3)
  }
  if(!log_scale){
    p <- p + scale_x_continuous(name = x_info$label, breaks = x_info$axis$breaks, minor = x_info$axis$minor_breaks)
  }else{
    p <- p + scale_x_continuous(name = x_info$label, breaks = x_info$axis$breaks, minor = x_info$axis$minor_breaks, trans = "log2")
  }

  ggsave(paste(filename, "pdf", sep = "."), width=dimensions[1], height=dimensions[2])
  ggsave(paste(filename, "png", sep = "."), width=dimensions[1], height=dimensions[2])
}

paper_plot <- function(
    plot_data, 
    filename, 
    x_info, 
    y_info, 
    group_info, 
    ld = FALSE, 
    log_scale = FALSE, 
    v_line_at = -1, 
    max_val = 10000, 
    dimensions = c(5.3, 2.2), 
    rects = list(), 
    tex = TRUE, 
    multi_line = FALSE,
    align_legend_left = FALSE, 
    y_limits = c(-1, -1)
){
    
    if(tex){
        tikz(paste(filename,"tex", sep = "."), width = dimensions[1], height = dimensions[2])
    }
    if(ld){
        plot_data$x_axis <- log(plot_data$x_axis, 2)
    }
    if(y_limits[1] < 0 | y_limits[2] < 0){
        y_limits = c(fmin(plot_data$y_axis), fmax(plot_data$y_axis))
    }
    # print(paste(group_info$color, names(group_info$color)))
    # print(unique(plot_data$print_name))
    # print(y_info$axis$breaks + 1)
    x_limits = c(fmin(plot_data$x_axis), fmax(plot_data$x_axis))
    p <- ggplot(data = plot_data, aes(x = x_axis, y = y_axis, group = print_name)) +
        geom_line(linewidth = 1, aes(color = print_name))+
        theme_light() + 
        scale_y_continuous(
            name = y_info$label, 
            breaks = y_info$axis$breaks,
            labels = fix_break_labels(y_info$axis$breaks, ld = FALSE), 
            minor = y_info$axis$minor_breaks, 
            limits = y_limits
        )+
        scale_color_manual(name = group_info$label, values = group_info$color)
        
    if(multi_line){
        p <- p + guides(color=guide_legend(ncol=2,byrow=TRUE))
    }

    if(align_legend_left == TRUE){
        p <- p + theme(legend.position = "top",
            panel.grid.major = element_line(linewidth = 0.6, color = "grey80"),
            panel.grid.minor = element_line(linetype = 'solid', linewidth = 0.2),
            plot.margin=unit(c(0, 0.5, 0.3, 0.3), units="lines"),
            legend.margin=margin(),
            legend.key.spacing.y = unit(-0.25, "lines"),
            # legend.box.margin=margin(-10,-10,-10,-10),
            # plot.background=element_rect(fill="red"),
            legend.justification='left',
            legend.key.width = unit(0.5, 'lines'),
            legend.box.spacing = unit(0, "lines")
        )
    }else{
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
    }
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
        # labels <- x_info$axis$breaks
        # str <- paste(labels)
        # str2 <- paste("$2^{",log(labels, 2), "}$", sep = "")
        # final_labels <- str
        # final_labels[labels > max_val] <- str2[labels > max_val]
        # final_labels <- TeX(final_labels)
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

paper_plot_2_y <- function(plot_data, filename, x_info, all_y_info, group_info, ld = FALSE, log_scale = FALSE, v_line_at = -1, max_val = 4096, dimensions = c(5.3, 2.2), rects = list(), tex = TRUE){
    # print(all_y_info)
    if(tex){
        tikz(paste(filename,"tex", sep = "."), width = dimensions[1], height = dimensions[2])
    }
    if(ld){
        plot_data$x_axis <- log(plot_data$x_axis, 2)
    }
    # print(all_y_info)
    coeff <- all_y_info$axis$coeff
    # print(paste(all_y_info$label2, coeff))

    linetypelegend <- c(1,3)
    names(linetypelegend) <- c(all_y_info$label1, all_y_info$label2)

    x_limits = c(fmin(plot_data$x_axis), fmax(plot_data$x_axis))
    p <- ggplot(data = plot_data, aes(x = x_axis, group = print_name)) +
        geom_line(linewidth = 1, aes(y=y1_axis, linetype=all_y_info$label1, color = print_name))+
        geom_line(linewidth = 1, aes(y=y2_axis * coeff, linetype=all_y_info$label2, color = print_name))+
        theme_light() + 
        scale_y_continuous(
            name = all_y_info$label1, 
            breaks = all_y_info$axis$breaks,
            labels = fix_break_labels(all_y_info$axis$breaks, ld = FALSE), 
            minor = all_y_info$axis$minor_breaks,
            sec.axis = sec_axis(~ . / coeff, 
                name = "Counter Value", 
                breaks = all_y_info$axis$breaks_y2, 
                labels = fix_break_labels(all_y_info$axis$breaks_y2, ld = FALSE, 1000)
            )
            # TODO if you don't want the x-axis to say counter value change it here. all_y_info$label2 is a good option
        )+
        scale_color_manual(name = group_info$label, values = group_info$color)+
        scale_linetype_manual(name = "", values = linetypelegend)+
        theme(legend.position = "top",
            panel.grid.major = element_line(linewidth = 0.6, color = "grey80"),
            panel.grid.minor = element_line(linetype = 'solid', linewidth = 0.2),
            legend.margin=margin(1,1,1,1),
            legend.box.margin=margin(-5,-10,-10,-10))
    
    for(r in rects){
        helper <- plot_data[plot_data$print_name %in% r$algs,]
        current_ymin <- fmin(helper[helper$x_axis >= r$xmin & helper$x_axis <= r$xmax,]$y_axis) * 0.9
        current_ymax <- fmax(helper[helper$x_axis >= r$xmin & helper$x_axis <= r$xmax,]$y_axis) * 1.03
        p <- p + annotate("rect", xmin = r$xmin, xmax = r$xmax, ymin = current_ymin, ymax = current_ymax, alpha = 0, color="grey25", linetype =2, linewidth = 0.4) +
            annotate("text", x = r$xmin * r$xk + (1 - r$xk) * r$xmax, y = current_ymin * r$yk + current_ymax * (1 - r$yk), color ="grey25", label = r$text, size = 2.5) 
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
        # labels <- x_info$axis$breaks
        # str <- paste(labels)
        # str2 <- paste("$2^{",log(labels, 2), "}$", sep = "")
        # final_labels <- str
        # final_labels[labels > max_val] <- str2[labels > max_val]
        # final_labels <- TeX(final_labels)
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
    # exit()
}

general_bar_plot <- function(plot_data, filename, x_info, y_info, group_info, ld = FALSE, log_scale = FALSE){
    if(ld){
        plot_data$x_axis <- log(plot_data$x_axis, 2)
    }
    p <- ggplot(data = plot_data, aes(x = x_axis, y = y_axis, group = byte_count)) +
        geom_col(position = "dodge2", aes(color = byte_count, fill = byte_count))+
        theme_light() + 
        scale_x_discrete(name = x_info$label)+
        scale_y_continuous(name = y_info$label, breaks = y_info$axis$breaks, minor = y_info$axis$minor_breaks)+
        scale_color_discrete(name = group_info$label)+
        scale_fill_discrete(name = group_info$label)+
        theme(legend.position.inside = c(0, 7, 0, 7),
            panel.grid.major = element_line(linewidth = 0.6, color = "grey80"),
            panel.grid.minor = element_line(linetype = 'dashed', linewidth = 0.2))
    
    # if(!log_scale){
    # }else{
    #     p <- p + scale_x_continuous(name = x_info$label, breaks = x_info$axis$breaks, minor = x_info$axis$minor_breaks, trans = "log2")
    # }
    p + coord_flip()
    ggsave(paste(filename, "pdf", sep = "."), width=plot_width, height=plot_heigth)
    ggsave(paste(filename, "png", sep = "."), width=plot_width, height=plot_heigth)
}

# EXPLICIT FUNCTIONS FOR experiment_02_visualize1 and experiment_02_visualize2
plot_lanes <- function(agg_data, to_summarise, file_data){
        colnames(agg_data)[which(colnames(agg_data) == "lane_count")] <- "x_axis"
        pre_plot_data_plus_info <- better_representation_of_few_data_points(agg_data)
        pre_plot_data <- pre_plot_data_plus_info$frame
        simple_line_labels <- pre_plot_data_plus_info$info

        x_label <- "Lane Count"
        x_axis <- get_breaks(pre_plot_data$x_axis, number_x_axis_ticks, power_two = TRUE)

        all_options <- unique(pre_plot_data$sub)
        for(option in all_options){
            sub_pre_plot_data <- pre_plot_data[pre_plot_data$sub == option,]
            if(nrow(sub_pre_plot_data ) > 16 * length(unique(sub_pre_plot_data$algorithm))){
                for(metric in to_summarise){
                    plot_data <- sub_pre_plot_data[,which(colnames(sub_pre_plot_data) %in%c("print_name", "x_axis", metric))]
                    colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
                    y_label <- metric
                    if(metric == "gibs"){y_label <- "GiB / s"}
                    y_axis <- get_breaks(plot_data$y_axis, number_y_axis_ticks, start_at_zero = FALSE)
                    lt_values <- linetype_value(plot_data$print_name, simple_line_labels)

                    general_plot(plot_data, paste(file_data$filename, "/", paste("sub", option, metric, sep = "_"), sep = ""),
                        list(label = x_label, axis = x_axis),
                        list(label = y_label, axis = y_axis),
                        list(label = "Algorithm", values = lt_values))
                }
            }
        }
}

plot_fewer <- function(agg_data, to_summarise, file_data){
        colnames(agg_data)[which(colnames(agg_data) == "sub")] <- "x_axis"
        pre_plot_data_plus_info <- better_representation_of_few_data_points(agg_data)
        pre_plot_data <- pre_plot_data_plus_info$frame
        simple_line_labels <- pre_plot_data_plus_info$info

        x_label <- "Fewer Elements"
        x_axis <- get_breaks(pre_plot_data$x_axis, number_x_axis_ticks, power_two = TRUE)

        all_options <- unique(pre_plot_data$lane_count)
        for(option in all_options){
            sub_pre_plot_data <- pre_plot_data[pre_plot_data$lane_count == option,]
            if(nrow(sub_pre_plot_data ) > 256 * length(unique(sub_pre_plot_data$algorithm))){
                for(metric in to_summarise){
                    plot_data <- sub_pre_plot_data[,which(colnames(sub_pre_plot_data) %in%c("print_name", "x_axis", metric))]
                    colnames(plot_data)[which(colnames(plot_data) == metric)] <- "y_axis"
                    y_label <- metric
                    if(metric == "gibs"){y_label <- "GiB / s"}
                    y_axis <- get_breaks(plot_data$y_axis, number_y_axis_ticks, start_at_zero = FALSE)
                    lt_values <- linetype_value(plot_data$print_name, simple_line_labels)

                    general_plot(plot_data, paste(file_data$filename, "/", paste("lane", option, metric, sep = "_"), sep = ""),
                        list(label = x_label, axis = x_axis),
                        list(label = y_label, axis = y_axis),
                        list(label = "Algorithm", values = lt_values))
                }
            }
        }
}

