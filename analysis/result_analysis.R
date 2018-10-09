# ==============================================================================
# Clear workspace

rm(list = ls())


# ==============================================================================
# Install dependencies

dependencies.list <- c(
  "dplyr",
  "ggplot2",
  "boot",
  "ggrepel",
  "xtable",
  "scales",
  "extrafont"
)

dependencies.missing <- dependencies.list[!(dependencies.list %in% installed.packages()[,"Package"])]
if (length(dependencies.missing) > 0) {
  
  # Notify for missing libraries
  print("The following packages are required but are not installed:")
  print(dependencies.missing)
  dependencies.install <- readline(prompt = "Do you want them to be installed (Y/n)? ")
  if (any(tolower(dependencies.install) == c("y", "yes"))) {
    install.packages(dependencies.missing)
  }
}


# ==============================================================================
# Load libraries

suppressMessages(library(dplyr))
suppressMessages(library(ggplot2))
suppressMessages(library(boot))
suppressMessages(library(ggrepel))
suppressMessages(library(xtable))
suppressMessages(library(scales))


# ==============================================================================
# Read data from files

# Read data file
data.results <- read.csv("./data/results.csv", header = TRUE)

# Extract some information about instances encoded at their names
n <- c()
m <- c()
t <- c()

for (row in 1:nrow(data.results)) {
  name <- as.character(data.results[row, "INSTANCE"])
  name_attr <- unlist(strsplit(name, split="-"))

  n <- c(n, as.numeric(as.character(name_attr[2])))
  m <- c(m, as.numeric(as.character(name_attr[3])))
  t <- c(t, as.character(name_attr[4]))
}

data.results$N <- n                        # Number of switch maneuvers
data.results$M <- m                        # Number of teams available
data.results$PRECEDENCE_TREE <- factor(t)  # Class of precedence graph


# ==============================================================================
# Greedy heuristics: mean objective per heuristic and problem size

# Algorithms compared
algorithms.labels <- c("greedy", "neh")
algorithms.names  <- c("Simple Greedy Heuristic", "NEH-based Greedy Heuristic")
names(algorithms.names) <- algorithms.labels

# Precondition data
aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::arrange(INSTANCE, SEED, ALGORITHM) %>%
  dplyr::select(N, M, PRECEDENCE_TREE, ALGORITHM, SEED, OBJECTIVE) %>%
  dplyr::group_by(N, M, PRECEDENCE_TREE, ALGORITHM) %>%
  dplyr::summarise(MEAN.OBJECTIVE = mean(OBJECTIVE))

# Small instances
#pdf(file = "./figures/boxplot-mean-objective-small.pdf", width = 10, height = 7)
postscript(file = "./figures/boxplot-mean-objective-small.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(subset(aggdata, N <= 12), ggplot2::aes(x = ALGORITHM, y = MEAN.OBJECTIVE, fill = ALGORITHM))
fig + ggplot2::geom_boxplot() + 
  ggplot2::facet_grid(M~N, scales = "free_y", labeller = label_both) +
  ggplot2::xlab("") +
  ggplot2::ylab("Mean objective") +
  ggplot2::scale_fill_manual(name = "Heuristic: ", limits = algorithms.labels, labels = algorithms.names, values = c("#F0F0F0", "#787878")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_blank(),
    axis.text.x = ggplot2::element_blank(),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white"),
    strip.background = element_rect(fill = "white", linetype = "blank"),
    strip.text = ggplot2::element_text(size = 16))
dev.off()

# Large instances
#pdf(file = "./figures/boxplot-mean-objective-large.pdf", width = 10, height = 7)
postscript(file = "./figures/boxplot-mean-objective-large.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(subset(aggdata, N >= 50), ggplot2::aes(x = ALGORITHM, y = MEAN.OBJECTIVE, fill = ALGORITHM)) 
fig + ggplot2::geom_boxplot() + 
  ggplot2::facet_grid(M~N, scales = "free_y", labeller = label_both) +
  ggplot2::xlab("") +
  ggplot2::ylab("Mean objective") +
  ggplot2::scale_fill_manual(name = "Heuristic: ", limits = algorithms.labels, labels = algorithms.names, values = c("#F0F0F0", "#787878")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_blank(),
    axis.text.x = ggplot2::element_blank(),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white"),
    strip.background = element_rect(fill = "white", linetype = "blank"),
    strip.text = ggplot2::element_text(size = 16))
dev.off()


# ==============================================================================
# Greedy heuristics: mean runtime per heuristic and problem size

# Algorithms compared
algorithms.labels <- c("greedy", "neh")
algorithms.names  <- c("Simple Greedy Heuristic", "NEH-based Greedy Heuristic")
names(algorithms.names) <- algorithms.labels

# Precondition data
aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::arrange(INSTANCE, SEED, ALGORITHM) %>%
  dplyr::select(N, M, PRECEDENCE_TREE, ALGORITHM, SEED, TIME.SEC) %>%
  dplyr::group_by(N, M, PRECEDENCE_TREE, ALGORITHM) %>%
  dplyr::summarise(MEAN.TIME = mean(TIME.SEC))

# Small instances
#pdf(file = "./figures/boxplot-mean-runtime-small.pdf", width = 10, height = 7)
postscript(file = "./figures/boxplot-mean-runtime-small.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(subset(aggdata, N <= 12), ggplot2::aes(x = ALGORITHM, y = MEAN.TIME, fill = ALGORITHM))
fig + ggplot2::geom_boxplot() + 
  ggplot2::facet_grid(M~N, scales = "free_y", labeller = label_both) +
  ggplot2::xlab("") +
  ggplot2::ylab("Mean runtime (in seconds)") +
  ggplot2::scale_fill_manual(name = "Heuristic: ", limits = algorithms.labels, labels = algorithms.names, values = c("#F0F0F0", "#787878")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_blank(),
    axis.text.x = ggplot2::element_blank(),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white"),
    strip.background = element_rect(fill = "white", linetype = "blank"),
    strip.text = ggplot2::element_text(size = 16))
dev.off()

# Large instances
#pdf(file = "./figures/boxplot-mean-runtime-large.pdf", width = 10, height = 7)
postscript(file = "./figures/boxplot-mean-runtime-large.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(subset(aggdata, N >= 50), ggplot2::aes(x = ALGORITHM, y = MEAN.TIME, fill = ALGORITHM)) 
fig + ggplot2::geom_boxplot() + 
  ggplot2::facet_grid(M~N, scales = "free_y", labeller = label_both) +
  ggplot2::xlab("") +
  ggplot2::ylab("Mean runtime (in seconds)") +
  ggplot2::scale_fill_manual(name = "Heuristic: ", limits = algorithms.labels, labels = algorithms.names, values = c("#F0F0F0", "#787878")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_blank(),
    axis.text.x = ggplot2::element_blank(),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white"),
    strip.background = element_rect(fill = "white", linetype = "blank"),
    strip.text = ggplot2::element_text(size = 16))
dev.off()


# ==============================================================================
# LaTeX table with results of MIP and greedy heuristics

# Algorithms compared
algorithms.labels <- c("greedy", "neh", "mip-linear-ordering")
algorithms.names  <- c("Simple Greedy Heur.", "NEH-based Greedy Heur.", "MIP Formulation")
names(algorithms.names) <- algorithms.labels

# Precondition data
aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::filter(SEED == 2) %>%
  dplyr::arrange(ALGORITHM, INSTANCE, SEED) %>%
  dplyr::select(N, M, PRECEDENCE_TREE, ALGORITHM, SEED, OBJECTIVE, TIME.SEC)
  
heur.gap <- ((aggdata$OBJECTIVE - aggdata$OBJECTIVE[aggdata$ALGORITHM == "mip-linear-ordering"]) / 
               aggdata$OBJECTIVE[aggdata$ALGORITHM == "mip-linear-ordering"]) * 100.0
  
aggdata <- aggdata %>% 
  dplyr::bind_cols(GAP = heur.gap) %>% 
  dplyr::group_by(N, M, ALGORITHM) %>%
  dplyr::summarise(MEAN.OBJECTIVE = mean(OBJECTIVE), 
                   MEAN.TIME = mean(TIME.SEC),
                   MEAN.GAP = mean(GAP))

# Latex table
table.results <- with(aggdata,
                      cbind(N = aggdata$N[ALGORITHM == 'mip-linear-ordering'],
                            M = aggdata$M[ALGORITHM == 'mip-linear-ordering'],
                            MIP.OBJ = aggdata$MEAN.OBJECTIVE[ALGORITHM == 'mip-linear-ordering'],
                            MIP.GAP = aggdata$MEAN.GAP[ALGORITHM == 'mip-linear-ordering'],
                            NEH.OBJ = aggdata$MEAN.OBJECTIVE[ALGORITHM == 'neh'],
                            NEH.GAP = aggdata$MEAN.GAP[ALGORITHM == 'neh'],
                            NEH.TIME = aggdata$MEAN.TIME[ALGORITHM == 'neh'],
                            SIMPLE.OBJ = aggdata$MEAN.OBJECTIVE[ALGORITHM == 'greedy'],
                            SIMPLE.GAP = aggdata$MEAN.GAP[ALGORITHM == 'greedy'],
                            SIMPLE.TIME = aggdata$MEAN.TIME[ALGORITHM == 'greedy']))
                            
#rownames(my.table.best) <- sort(data.instances$NAME)
xtable(table.results, digits = c(6,0,0,2,4,2,4,4,2,4,4))


# ==============================================================================
# LaTeX table with results of the ILS heuristic

# Algorithms compared
algorithms.labels <- c("ils", "greedy", "mip-linear-ordering")
algorithms.names  <- c("ILS", "Simple Greedy Heuristic", "MIP (linear-ordering variavles)")
names(algorithms.names) <- algorithms.labels

# Precondition data
aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::arrange(INSTANCE, SEED, ALGORITHM) %>%
  dplyr::select(INSTANCE, N, M, PRECEDENCE_TREE, ALGORITHM, SEED, OBJECTIVE, TIME.SEC) %>%
  dplyr::group_by(INSTANCE, N, M, ALGORITHM) %>%
  dplyr::summarise(MEAN.TIME = mean(TIME.SEC),
                   MEAN.OBJECTIVE = mean(OBJECTIVE),
                   BEST.OBJECTIVE = min(OBJECTIVE),
                   WORST.OBJECTIVE = max(OBJECTIVE)) %>%
  dplyr::ungroup() %>%
  dplyr::group_by(N, M, ALGORITHM) %>%
  dplyr::summarise(MEAN.TIME = mean(MEAN.TIME),
                   MEAN.OBJECTIVE = mean(MEAN.OBJECTIVE),
                   MEAN.BEST.OBJECTIVE = mean(BEST.OBJECTIVE),
                   MEAN.WORST.OBJECTIVE = mean(WORST.OBJECTIVE))

# Calculate the percent improvement of the ILS from the start solution
aux.improv <- data.results %>%
  dplyr::filter(ALGORITHM %in% c('ils', 'greedy')) %>%
  dplyr::arrange(ALGORITHM, SEED, INSTANCE)

ILS.IMPROV <- ((aux.improv$OBJECTIVE[aux.improv$ALGORITHM == 'greedy'] - aux.improv$OBJECTIVE[aux.improv$ALGORITHM == 'ils']) / 
                 aux.improv$OBJECTIVE[aux.improv$ALGORITHM == 'greedy']) * 100.0

aux.improv <- aux.improv %>%
  dplyr::filter(ALGORITHM %in% c('ils')) %>%
  dplyr::mutate(IMPROV = ILS.IMPROV) %>%
  dplyr::group_by(N, M, ALGORITHM) %>%
  dplyr::summarise(MEAN.IMPROV = mean(IMPROV))

# Calculate the percent gap of the ILS to the MIP result
aux.gap <- data.results %>%
  dplyr::filter(ALGORITHM %in% c('ils', 'mip-linear-ordering')) %>%
  dplyr::arrange(ALGORITHM, SEED, INSTANCE) %>%
  dplyr::group_by(INSTANCE, N, M, ALGORITHM) %>%
  dplyr::summarise(BEST.OBJ = min(OBJECTIVE))

ILS.GAP <- ((aux.gap$BEST.OBJ[aux.gap$ALGORITHM == 'ils'] - aux.gap$BEST.OBJ[aux.gap$ALGORITHM == 'mip-linear-ordering']) / 
              aux.gap$BEST.OBJ[aux.gap$ALGORITHM == 'mip-linear-ordering']) * 100.0

aux.gap <- aux.gap %>%
  dplyr::filter(ALGORITHM %in% c('ils')) %>%
  dplyr::bind_cols(GAP = ILS.GAP) %>%
  dplyr::group_by(N, M, ALGORITHM) %>%
  dplyr::summarise(MEAN.GAP = mean(GAP))

# Latex table
table.results <- with(aggdata,
                      cbind(N = aggdata$N[ALGORITHM == 'ils'],
                            M = aggdata$M[ALGORITHM == 'ils'],
                            MIP.MEAN.OBJ = aggdata$MEAN.OBJECTIVE[ALGORITHM == 'mip-linear-ordering'],
                            START.MEAN.OBJ = aggdata$MEAN.OBJECTIVE[ALGORITHM == 'greedy'],
                            ILS.MEAB.BEST.OBJ = aggdata$MEAN.BEST.OBJECTIVE[ALGORITHM == 'ils'],
                            ILS.MEAN.WORST.OBJ = aggdata$MEAN.WORST.OBJECTIVE[ALGORITHM == 'ils'],
                            ILS.MEAN.OBJ = aggdata$MEAN.OBJECTIVE[ALGORITHM == 'ils'],
                            ILS.MEAN.IMPROV = aux.improv$MEAN.IMPROV,
                            ILS.MEAN.GAP = aux.gap$MEAN.GAP,
                            ILS.MEAN.TIME = aggdata$MEAN.TIME[ALGORITHM == 'ils']))

#rownames(my.table.best) <- sort(data.instances$NAME)
xtable(table.results, digits = c(6,0,0,2,2,2,2,2,2,2,4))


# ==============================================================================
# Bar plots with proportion of instances solved and optimally solved (stratified by instance size)

# Algorithms compared
algorithms.labels <- c("mip-linear-ordering", "mip-precedence", "mip-arc-time-indexed")
algorithms.names  <- c("Linear Ordering", "Precedence", "Arc-Time-Indexed")
names(algorithms.names) <- algorithms.labels

# Precondition data
TOTAL <- length(unique(data.results$INSTANCE))

aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::filter(STATUS %in% c('OPTIMAL', 'SUBOPTIMAL')) %>%
  dplyr::arrange(INSTANCE, SEED, ALGORITHM) %>%
  dplyr::select(N, M, ALGORITHM, STATUS) %>%
  dplyr::group_by(N, M, ALGORITHM, STATUS) %>%
  dplyr::summarise(COUNT = n())

#pdf(file = "./figures/bars-stats-small.pdf", width = 10, height = 7)
postscript(file = "./figures/bars-stats-small.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(subset(aggdata, N <= 12), ggplot2::aes(x = ALGORITHM, y = 100 * (COUNT / (TOTAL / 24)), fill = STATUS)) 
fig + ggplot2::geom_bar(stat = 'identity', colour = "black") + 
  ggplot2::facet_grid(M~N, scales = "fixed", labeller = label_both) + 
  ggplot2::xlab("MIP Formulation") +
  ggplot2::ylab("Percentage") +
  ggplot2::scale_x_discrete(limits = algorithms.labels, labels = c('Linear Ord.', 'Precedence', 'Arc-Time-Idx.')) +
  ggplot2::scale_fill_manual(name = "Status: ", limits = c('SUBOPTIMAL', 'OPTIMAL'), labels = c('Sub-optimal', 'Optimal'), values = c("#787878", "#F0F0F0")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_text(size = 18, margin = ggplot2::margin(t = 15)),
    axis.text.x = ggplot2::element_text(size = 16, margin = ggplot2::margin(t = 5), angle = 45, hjust = 1),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white"),
    strip.background = element_rect(fill = "white", linetype = "blank"),
    strip.text = ggplot2::element_text(size = 16)
    )
dev.off()

#pdf(file = "./figures/bars-stats-large.pdf", width = 10, height = 7)
postscript(file = "./figures/bars-stats-large.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(subset(aggdata, N >= 50), ggplot2::aes(x = ALGORITHM, y = 100 * (COUNT / (TOTAL / 24)), fill = STATUS)) 
fig + ggplot2::geom_bar(stat = 'identity', colour = "black") + 
  ggplot2::facet_grid(M~N, scales = "fixed", labeller = label_both) + 
  ggplot2::xlab("MIP Formulation") +
  ggplot2::ylab("Percentage") +
  ggplot2::scale_x_discrete(limits = algorithms.labels, labels = c('Linear Ord.', 'Precedence', 'Arc-Time-Idx.')) +
  ggplot2::scale_fill_manual(name = "Status: ", limits = c('SUBOPTIMAL', 'OPTIMAL'), labels = c('Sub-optimal', 'Optimal'), values = c("#787878", "#F0F0F0")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_text(size = 18, margin = ggplot2::margin(t = 15)),
    axis.text.x = ggplot2::element_text(size = 16, margin = ggplot2::margin(t = 5), angle = 45, hjust = 1),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white"),
    strip.background = element_rect(fill = "white", linetype = "blank"),
    strip.text = ggplot2::element_text(size = 16)
  )
dev.off()
  

# ==============================================================================
# Bar plots with proportion of instances solved and optimally solved (aggredated)

# Algorithms compared
algorithms.labels <- c("mip-linear-ordering", "mip-precedence", "mip-arc-time-indexed")
algorithms.names  <- c("Linear Ordering", "Precedence", "Arc-Time-Indexed")
names(algorithms.names) <- algorithms.labels

# Precondition data
TOTAL <- length(unique(data.results$INSTANCE))

aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::filter(STATUS %in% c('OPTIMAL', 'SUBOPTIMAL')) %>%
  dplyr::arrange(INSTANCE, SEED, ALGORITHM) %>%
  dplyr::select(N, M, ALGORITHM, STATUS) %>%
  dplyr::group_by(ALGORITHM, STATUS) %>%
  dplyr::summarise(COUNT = n())

#pdf(file = "./figures/bars-stats-overall.pdf", width = 10, height = 7)
postscript(file = "./figures/bars-stats-overall.eps", width = 10, height = 7, paper = "special", horizontal = FALSE)
fig <- ggplot2::ggplot(aggdata, ggplot2::aes(x = ALGORITHM, y = 100 * (COUNT / TOTAL), fill = STATUS)) 
fig + ggplot2::geom_bar(stat = 'identity', colour = "black") + 
  ggplot2::xlab("MIP Formulation") +
  ggplot2::ylab("Percentage") +
  ggplot2::scale_x_discrete(limits = algorithms.labels, labels = c('Linear Ord.', 'Precedence', 'Arc-Time-Idx.')) +
  ggplot2::scale_fill_manual(name = "Status: ", limits = c('SUBOPTIMAL', 'OPTIMAL'), labels = c('Sub-optimal', 'Optimal'), values = c("#787878", "#F0F0F0")) +
  ggplot2::theme_bw() +
  ggplot2::theme(
    #panel.background = ggplot2::element_blank(),
    #panel.border = ggplot2::element_rect(colour = "black", fill = NA, size = 1)
    panel.spacing = ggplot2::unit(10, units = "pt"),
    axis.title.x = ggplot2::element_text(size = 18, margin = ggplot2::margin(t = 15)),
    axis.text.x = ggplot2::element_text(size = 16, margin = ggplot2::margin(t = 5), angle = 45, hjust = 1),
    axis.title.y = ggplot2::element_text(size = 18, margin = ggplot2::margin(r = 15)),
    axis.text.y = ggplot2::element_text(size = 16),
    legend.position = "bottom",
    legend.title = ggplot2::element_text(size = 18),
    legend.text = ggplot2::element_text(size = 18),
    legend.key = element_rect(size = 5, color = "white")
  )
dev.off()


# ==============================================================================
# LaTeX table with results of the MIP formulations

# Algorithms compared
algorithms.labels <- c("mip-linear-ordering", "mip-precedence", "mip-arc-time-indexed")
algorithms.names  <- c("Linear Ordering", "Precedence", "Arc-Time-Indexed")
names(algorithms.names) <- algorithms.labels

# Precondition data
aggdata <- data.results %>%
  dplyr::filter(ALGORITHM %in% algorithms.labels) %>%
  dplyr::filter(STATUS %in% c('OPTIMAL', 'SUBOPTIMAL')) %>%
  dplyr::arrange(INSTANCE, SEED, ALGORITHM) %>%
  dplyr::mutate(OPT.GAP = round(OPT.GAP, 4)) %>%
  dplyr::select(N, M, ALGORITHM, OBJECTIVE, OPT.GAP, RELAXATION, ITERATIONS, TIME.SEC) %>%
  dplyr::group_by(N, M, ALGORITHM) %>%
  dplyr::summarise(MEAN.OBJ = mean(OBJECTIVE),
                   MEAN.GAP = mean(OPT.GAP * 100),
                   MEAN.REL = mean(RELAXATION),
                   MEAN.NODES = mean(ITERATIONS),
                   MEAN.TIME = mean(TIME.SEC))

# Latex table
table.results <- with(aggdata,
                      cbind(N = aggdata$N[ALGORITHM == 'mip-linear-ordering'],
                            M = aggdata$M[ALGORITHM == 'mip-linear-ordering'],
                            
                            LINORD.OBJ = aggdata$MEAN.OBJ[ALGORITHM == 'mip-linear-ordering'],
                            LINORD.GAP = aggdata$MEAN.GAP[ALGORITHM == 'mip-linear-ordering'],
                            LINORD.REL = aggdata$MEAN.REL[ALGORITHM == 'mip-linear-ordering'],
                            LINORD.NODES = aggdata$MEAN.NODES[ALGORITHM == 'mip-linear-ordering'],
                            LINORD.TIME = aggdata$MEAN.TIME[ALGORITHM == 'mip-linear-ordering'],
                            
                            PREC.OBJ = aggdata$MEAN.OBJ[ALGORITHM == 'mip-precedence'],
                            PREC.GAP = aggdata$MEAN.GAP[ALGORITHM == 'mip-precedence'],
                            PREC.REL = aggdata$MEAN.REL[ALGORITHM == 'mip-precedence'],
                            PREC.NODES = aggdata$MEAN.NODES[ALGORITHM == 'mip-precedence'],
                            PREC.TIME = aggdata$MEAN.TIME[ALGORITHM == 'mip-precedence'],
                            
                            ARCTI.OBJ = aggdata$MEAN.OBJ[ALGORITHM == 'mip-arc-time-indexed'],
                            ARCTI.GAP = aggdata$MEAN.GAP[ALGORITHM == 'mip-arc-time-indexed'],
                            ARCTI.REL = aggdata$MEAN.REL[ALGORITHM == 'mip-arc-time-indexed'],
                            ARCTI.NODES = aggdata$MEAN.NODES[ALGORITHM == 'mip-arc-time-indexed'],
                            ARCTI.TIME = aggdata$MEAN.TIME[ALGORITHM == 'mip-arc-time-indexed']))

#rownames(my.table.best) <- sort(data.instances$NAME)
xtable(table.results, digits = c(6,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2))

