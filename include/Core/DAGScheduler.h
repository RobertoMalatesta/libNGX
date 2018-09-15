namespace ngx::Core {


    class ExecutionState {
        protected:
            uint32_t Stage;
            uint32_t AllTasks;
            uint32_t RunningTasks;
            uint32_t ErrorTasks;
    }

    class ExecutionStage {
        private:
            PromiseCallback *Tasks[32];
    };

    class DAGExecutionGraph {
    };
}