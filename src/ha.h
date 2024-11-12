class HomeAssistantSensor {
    public:
        void track();
        void track(bool state);
    private:
        bool state = false;
        bool reconnect = true;
        bool static isValidConfig();
        void connect();
};
