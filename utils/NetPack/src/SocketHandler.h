class SocketHandler {
public:
	SocketHandler();
	virtual ~SocketHandler();
	virtual void check_socket();
protected:
	virtual void set_fd() {};
	virtual void check_fs() {};
	fd_set readfs;
	fd_set exceptfs; 
};	
