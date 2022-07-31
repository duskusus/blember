class IBO {
    public:
    IBO(unsigned int *p_data, unsigned int p_size);
    unsigned int *data;
    unsigned int size;
    unsigned int id;
    void bind();
    unsigned int getID();

};