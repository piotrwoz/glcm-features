# glcm-features

An university project for Geoinformatics Data Classification Methods classes at 2nd semester of Informatics Master's studies at Gdansk University of Technology.

## Project purpose

Project's target is to check usefulness of features calculated from Gray Level Co-occurrence Matrix (GLCM) of satellite images to determine their usefulness for classification of different objects from source images.

Currently supported features:
$$Energy = \sum_{i=0,j=0}^{M-1} [P(i,j)]^{2}$$
$$Entropy = -\sum_{i=0,j=0}^{M-1} [P(i,j) \cdot \log_2(P(i,j))]$$
$$Homogeneity = \sum_{i=0,j=0}^{M-1} [\frac{P(i,j)}{1 + (i - j)^2}]$$
$$Contrast = \sum_{i=0,j=0}^{M-1} [P(i,j) \cdot (i - j)^2]$$

Where $P(i, j)$ is normalised value of GLCM in $(i, j)$ point.
