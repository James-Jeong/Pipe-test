include makefile.conf

all: $(TARGET_NAME)

$(TARGET_NAME): $(TARGET_OBJS)
	$(CC) $(OFLAG) $@ $^

red:
	./$(TARGET_NAME) 2 > $(OUT)

clean:
	$(RM) $(TARGET_OBJS)
	$(RM) $(TARGET_NAME)
	$(RM) $(OUT)

