set(Eigen_CXX23_HEADERS Tensor TensorSymmetry ThreadPool)

install(FILES
  ${Eigen_CXX23_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/Eigen/CXX23 COMPONENT Devel
  )

install(DIRECTORY src DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/Eigen/CXX23 COMPONENT Devel FILES_MATCHING PATTERN "*.h")
