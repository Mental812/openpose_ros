#include <openpose.h>

using namespace openpose_ros;

OpenPose::OpenPose() : outputSize(op::flagsToPoint(FLAGS_output_resolution, "-1x-1")),
                       netInputSize(op::flagsToPoint(FLAGS_net_resolution, "-1x368")),
                       faceNetInputSize(op::flagsToPoint(FLAGS_face_net_resolution, "368x368 (multiples of 16)")),
                       handNetInputSize(op::flagsToPoint(FLAGS_hand_net_resolution, "368x368 (multiples of 16)")),
                       poseMode(op::flagsToPoseMode(FLAGS_body)),
                       poseModel(op::flagsToPoseModel(FLAGS_model_pose)),
                       keypointScaleMode(op::flagsToScaleMode(FLAGS_keypoint_scale)),
                       heatMapTypes(op::flagsToHeatMaps(FLAGS_heatmaps_add_parts, FLAGS_heatmaps_add_bkg,
                                                        FLAGS_heatmaps_add_PAFs)),
                       heatMapScaleMode(op::flagsToHeatMapScaleMode(FLAGS_heatmaps_scale)),
                       multipleView((FLAGS_3d || FLAGS_3d_views > 1)),
                       faceDetector(op::flagsToDetector(FLAGS_face_detector)),
                       handDetector(op::flagsToDetector(FLAGS_hand_detector)),
                       enableGoogleLogging(true),
                       opWrapper(op::ThreadManagerMode::Asynchronous),
                       wrapperStructPose(poseMode, netInputSize, outputSize, keypointScaleMode, FLAGS_num_gpu, FLAGS_num_gpu_start,
                                         FLAGS_scale_number, (float)FLAGS_scale_gap, op::flagsToRenderMode(FLAGS_render_pose, multipleView),
                                         poseModel, !FLAGS_disable_blending, (float)FLAGS_alpha_pose, (float)FLAGS_alpha_heatmap,
                                         FLAGS_part_to_show, FLAGS_model_folder, heatMapTypes, heatMapScaleMode, FLAGS_part_candidates,
                                         (float)FLAGS_render_threshold, FLAGS_number_people_max, FLAGS_maximize_positives, FLAGS_fps_max,
                                         FLAGS_prototxt_path, FLAGS_caffemodel_path, (float)FLAGS_upsampling_ratio, enableGoogleLogging),
                       wrapperStructFace(FLAGS_face, faceDetector, faceNetInputSize,
                                         op::flagsToRenderMode(FLAGS_face_render, multipleView, FLAGS_render_pose),
                                         (float)FLAGS_face_alpha_pose, (float)FLAGS_face_alpha_heatmap, (float)FLAGS_face_render_threshold),
                       wrapperStructHand(FLAGS_hand, handDetector, handNetInputSize, FLAGS_hand_scale_number, (float)FLAGS_hand_scale_range,
                                         op::flagsToRenderMode(FLAGS_hand_render, multipleView, FLAGS_render_pose), (float)FLAGS_hand_alpha_pose,
                                         (float)FLAGS_hand_alpha_heatmap, (float)FLAGS_hand_render_threshold),
                       wrapperStructExtra(FLAGS_3d, FLAGS_3d_min_views, FLAGS_identification, FLAGS_tracking, FLAGS_ik_threads),
                       wrapperStructOutput(FLAGS_cli_verbose, FLAGS_write_keypoint, op::stringToDataFormat(FLAGS_write_keypoint_format),
                                           FLAGS_write_json, FLAGS_write_coco_json, FLAGS_write_coco_json_variants, FLAGS_write_coco_json_variant,
                                           FLAGS_write_images, FLAGS_write_images_format, FLAGS_write_video, FLAGS_write_video_fps,
                                           FLAGS_write_video_with_audio, FLAGS_write_heatmaps, FLAGS_write_heatmaps_format, FLAGS_write_video_3d,
                                           FLAGS_write_video_adam, FLAGS_write_bvh, FLAGS_udp_host, FLAGS_udp_port)
                       // wrapperStructGui(op::flagsToDisplayMode(FLAGS_display, FLAGS_3d), !FLAGS_no_gui_verbose, FLAGS_fullscreen)


{
    if (!FLAGS_write_keypoint.empty())
    {
      op::log("Flag `write_keypoint` is deprecated and will eventually be removed."
              " Please, use `write_json` instead.", op::Priority::Max);
    }
            

    // Logging
    op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);

    // Configuring OpenPose
    op::log("Configuring OpenPose...", op::Priority::High);
    opWrapper.configure(wrapperStructPose);
    opWrapper.configure(wrapperStructFace);
    opWrapper.configure(wrapperStructHand);
    opWrapper.configure(wrapperStructExtra);
    opWrapper.configure(wrapperStructOutput);
    // opWrapper.configure(wrapperStructGui);

    // Set to single-thread running (to debug and/or reduce latency)
    if (FLAGS_disable_multi_thread)
    {
      opWrapper.disableMultiThreading();
    }

}

void OpenPose::start()
{
    opWrapper.start();
}

bool OpenPose::waitAndEmplace(std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>> &datumToProcess)
{
    opWrapper.waitAndEmplace(datumToProcess);
}

bool OpenPose::waitAndPop(std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>> &datumProcessed)
{
    opWrapper.waitAndPop(datumProcessed);
}

void OpenPose::stop()
{
    opWrapper.stop();
}