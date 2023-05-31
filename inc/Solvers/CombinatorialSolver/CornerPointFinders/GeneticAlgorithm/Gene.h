#ifndef GENE_H
#define GENE_H

class Gene
{
public:
    int position;

    Gene(int position)
        : position(position) {}

    int getPosition() const {
        return position;
    }

    bool operator==(const Gene& other) const
    {
        return position == other.position;
    }
};

#endif // GENE_H